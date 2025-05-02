

#ifdef RENDERER_VULKAN
namespace UpsilonEngine
{
	URenderBlock::URenderBlock()
	{
		m_name = "RenderBlock";
	}

	URenderBlock::~URenderBlock() {
	}

	bool URenderBlock::init()
	{
		m_renderer = UServiceLocator::getInstance()->getRenderer();
		glGenBuffers(1, &m_bufferId);
		glGenVertexArrays(1, &m_vertexArrayObject);

		return true;
	}

	void URenderBlock::onDestroy() {
	}

	void URenderBlock::update()
	{
		if (m_animationBufferDirty)
		{
			dumpAnimationData();
			m_animationBufferDirty = false;
		}

		/// Dirty on mesh update
		if (m_bufferDirty)
		{
			dumpBufferData();
			m_bufferDirty = false;
		}
	}

	void URenderBlock::render()
	{
		if (m_mesh == nullptr || m_material == nullptr)
		{
			LogError("Render block missing critical component, mesh or material.");
			return;
		}

		if (m_bufferDirty || m_mesh->m_vertices == nullptr)
		{
			return;
		}

		LogGLErrors();

		if (m_material)
		{

			// Ee bind the animation TBO to 31 so it's out of the way
			//if (m_animationBufferTextureId)
			//{
			//	glActiveTexture(GL_TEXTURE31);
			//	glBindTexture(GL_TEXTURE_BUFFER, m_animationBufferTextureId);
			//}

			m_material->bind();



			if (m_mesh->m_animationsRef != nullptr)
			{
				if (m_mesh->m_animationsRef->size() > 0)
				{
					float time = UServiceLocator::getInstance()->getTimeService()->getTime();
					m_material->getShader().setUniformfv("animationTimeElapsed", time);

					int count = 0;

					// Populate animations
					for (SharedPtr<UAnimation> anim : *m_mesh->m_animationsRef)
					{
						if (m_animationBuffer)
						{
							m_material->getShader().bindShaderStorageBuffer("Animations", m_animationBuffer);
							m_material->getShader().setUniform1i("animationCount", anim->getBoneAnimations().size());
						}

						for (SharedPtr<UBoneAnim> boneAnim : anim->getBoneAnimations())
						{
							for (int i = 0; i < 2; ++i)
							{
								std::string posKeyKey = "positionKeys[" + std::to_string(count * 2 + i) + "].Key";
								std::string posKeyTime = "positionKeys[" + std::to_string(count * 2 + i) + "].Time";

								std::string scaleKeyKey = "scaleKeys[" + std::to_string(count * 2 + i) + "].Key";
								std::string scaleKeyTime = "scaleKeys[" + std::to_string(count * 2 + i) + "].Time";

								std::string rotationKeyKey = "rotationKeys[" + std::to_string(count * 2 + i) + "].Key";
								std::string rotationKeyTime = "rotationKeys[" + std::to_string(count * 2 + i) + "].Time";

								int index = (int)time % anim->getBoneAnimations().size();

								// Wrap around.
								if (index + i >= boneAnim->getPositionKeys().size())
								{
									index = 0;
								}

								m_material->getShader().setUniformVec4(posKeyKey, boneAnim->getPositionKeys()[index + i].Value);
								m_material->getShader().setUniformfv(posKeyTime, boneAnim->getPositionKeys()[index + i].Time);

								m_material->getShader().setUniformVec4(rotationKeyKey, boneAnim->getRotationKeys()[index + i].Value);
								m_material->getShader().setUniformfv(rotationKeyTime, boneAnim->getRotationKeys()[index + i].Time);

								m_material->getShader().setUniformVec4(scaleKeyKey, boneAnim->getScaleKeys()[index + i].Value);
								m_material->getShader().setUniformfv(scaleKeyTime, boneAnim->getScaleKeys()[index + i].Time);
							}

							count++;
						}

						break;
					}
				}
			}

			if (m_renderer->isShadowPass())
			{
				ULight* activeLightSource = m_renderer->getActiveLight();
				float near_plane = 0.1f, far_plane = 1000.0f;
				//glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
				glm::mat4 lightProjection;

				// Spot light projection
				if (activeLightSource->getLightType() == LightType::Spot)
				{
					lightProjection = glm::perspectiveLH(activeLightSource->getApature() * 2, 1.0f, near_plane, far_plane);
				}
				else
				{
					lightProjection = glm::perspectiveLH(180.0f, 1.0f, near_plane, far_plane);
				}

				// Get active light and work out view matrix 
				glm::vec3 position = activeLightSource->getTransform()->getWorldPosition();
				glm::vec3 up = activeLightSource->getTransform()->getUp();
				glm::vec3 forward = activeLightSource->getTransform()->getForward();

				glm::mat4 view = glm::lookAtLH(position, position + forward, up);

				m_material->getShader().setUniformBool(m_shadowUniform, true);
				//m_material->getShader().uniformMat4fv(m_shadowMat, 1, false,
				//	lightProjection * view * m_transform->getWorldMatrix());
				m_material->getShader().uniformMat4fv(m_viewUniform, 1, false, view);
				m_material->getShader().uniformMat4fv(m_modelUniform, 1, false, m_transform->getWorldMatrix());
				m_material->getShader().uniformMat4fv(m_projUniform, 1, false, lightProjection);
			}
			else
			{
				if (m_transform != nullptr)
				{
					// Remove this cheaper todo in shader.
					//m_material->getShader().uniformMat4fv(wvpLoc, 1, false,
					//	m_renderer->getActiveCamera()->getProjMatrix() * m_renderer->getActiveCamera()->getViewMatrix() * m_transform->getWorldMatrix());

					m_material->getShader().uniformMat4fv(m_modelUniform, 1, false, m_transform->getWorldMatrix());
					m_material->getShader().uniformMat4fv(m_viewUniform, 1, false, m_renderer->getActiveCamera()->getViewMatrix());
					m_material->getShader().uniformMat4fv(m_projUniform, 1, false, m_renderer->getActiveCamera()->getProjMatrix());
				}

				m_material->getShader().setUniformBool(m_shadowUniform, false);
				m_material->getShader().setUniformVec3(m_cameraUniform, m_renderer->getActiveCamera()->getTransform()->getWorldPosition());
				m_material->getShader().setUniformVec3(m_cameraLook, m_renderer->getActiveCamera()->getTransform()->getForward());
			}
		}
		else
		{
			LogError("Render block no material present!");
		}

		LogGLErrors();

		// should not do blending here/ or use depth test here
		// See renderqueue in scene (sets depth test)

		if (m_mesh->shouldRenderBackFaces())
		{
			glDisable(GL_CULL_FACE);
		}
		else
		{
			glEnable(GL_CULL_FACE);
		}

		size_t textureBufferSize = m_mesh->getTextureBufferSize();
		size_t texture3dBufferSize = m_mesh->getTexture3dBufferSize();
		if (textureBufferSize == 0 && texture3dBufferSize == 0)
		{
			//m_material->getShader().setUniformBool("hasDiffuseMap", false);
			//m_material->getShader().setUniformBool("generatePlanarUvs", true);
		}

		glLineWidth(m_mesh->GetLineThickness());
		glBindVertexArray(m_vertexArrayObject);
		glDrawArrays(m_primitiveMode, 0, (GLsizei)m_mesh->m_vertices->size());

		LogGLErrors();
	}

	void URenderBlock::setDrawMode(uPrimitiveMode mode)
	{
		switch (mode) {

		case uPrimitiveMode::uPOINTS:
			m_primitiveMode = GL_POINTS;
			break;
		case uPrimitiveMode::uLINES:
			m_primitiveMode = GL_LINES;
			break;
		case uPrimitiveMode::uLINE_STRIP:
			m_primitiveMode = GL_LINE_STRIP;
			break;
		case uPrimitiveMode::uLINE_LOOP:
			m_primitiveMode = GL_LINE_LOOP;
			break;
		case uPrimitiveMode::uTRIANGLES:
			m_primitiveMode = GL_TRIANGLES;
			break;
		case uPrimitiveMode::uTRIANGLE_STRIP:
			m_primitiveMode = GL_TRIANGLE_STRIP;
			break;
		case uPrimitiveMode::uTRIANGLE_FAN:
			m_primitiveMode = GL_TRIANGLE_FAN;
			break;

		default:
			m_primitiveMode = GL_TRIANGLES;
		}
	}

	void URenderBlock::setVertexAttributes()
	{
		if (m_material == nullptr)
		{
			LogError("Unable to set vertex attributes, a valid material is required!");
			return;
		}

		glBindVertexArray(m_vertexArrayObject);
		glBindBuffer(GL_ARRAY_BUFFER, m_bufferId);
		LogGLErrors();
		const UShader shader = m_material->getShader();

		/// buffer offsets
		size_t vertexBufferSize = m_mesh->getVertexBufferSize();
		size_t textureBufferSize = m_mesh->getTextureBufferSize();
		size_t texture3dBufferSize = m_mesh->getTexture3dBufferSize();
		size_t colourBufferSize = m_mesh->getColourBufferSize();
		size_t tangentBufferSize = m_mesh->getTangentBufferSize();
		size_t biTangentBufferSize = m_mesh->getBiTangentBufferSize();
		size_t normalBufferSize = m_mesh->getNormalBufferSize();
		size_t weightSize = m_mesh->getWeightBufferSize();

		size_t tBufferSize = (texture3dBufferSize) ? texture3dBufferSize : textureBufferSize;

		// vertex positions
		vertexAttribPointer(shader.getProgram(), shader.VertexPositionAtrib, 3, GL_FLOAT, GL_FALSE, 0, 0);

		// uvs 
		if (texture3dBufferSize)
		{
			vertexAttribPointer(shader.getProgram(), shader.UVSAtrib, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(sizeof(float) * vertexBufferSize));
		}
		else
		{
			vertexAttribPointer(shader.getProgram(), shader.UVSAtrib, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(sizeof(float) * vertexBufferSize));
		}

		// colours 
		vertexAttribPointer(shader.getProgram(), shader.ColourAtrib, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(sizeof(float) * (vertexBufferSize + tBufferSize)));

		// normals
		vertexAttribPointer(shader.getProgram(), shader.NormalAtrib, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(sizeof(float) * (vertexBufferSize + tBufferSize + colourBufferSize)));

		// tangents
		vertexAttribPointer(shader.getProgram(), shader.TangentAtrib, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(sizeof(float) * (vertexBufferSize + tBufferSize + colourBufferSize + normalBufferSize)));

		// bit tangents 
		vertexAttribPointer(shader.getProgram(), shader.BiTangentAtrib, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(sizeof(float) * (vertexBufferSize + tBufferSize + colourBufferSize + normalBufferSize + tangentBufferSize)));

		// Bone weights
		vertexAttribPointer(shader.getProgram(), shader.WeightsAtrib, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(sizeof(float) * (vertexBufferSize + tBufferSize + colourBufferSize + normalBufferSize + tangentBufferSize + biTangentBufferSize)));

		vertexAttribPointer(shader.getProgram(), shader.BoneIdAtrib, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(sizeof(float) * (vertexBufferSize + tBufferSize + colourBufferSize + normalBufferSize + tangentBufferSize + biTangentBufferSize + weightSize)));

		enableVertexAttribArray(shader.getProgram(), shader.VertexPositionAtrib);
		enableVertexAttribArray(shader.getProgram(), shader.UVSAtrib);
		enableVertexAttribArray(shader.getProgram(), shader.NormalAtrib);
		enableVertexAttribArray(shader.getProgram(), shader.TangentAtrib);
		enableVertexAttribArray(shader.getProgram(), shader.BiTangentAtrib);
		enableVertexAttribArray(shader.getProgram(), shader.WeightsAtrib);
		enableVertexAttribArray(shader.getProgram(), shader.BoneIdAtrib);

		glBindVertexArray(0);
		LogGLErrors();
	}

	void URenderBlock::setMaterial(SharedPtr<UMaterial> Material)
	{
		m_material = Material;
		m_shadowMat = m_material->getShader().getUniformLocation("wvpShadowMat");
		m_modelUniform = m_material->getShader().getUniformLocation("modelMat");
		m_viewUniform = m_material->getShader().getUniformLocation("viewMat");
		m_projUniform = m_material->getShader().getUniformLocation("projMat");
		m_shadowUniform = m_material->getShader().getUniformLocation("shadowPass");
		m_cameraUniform = m_material->getShader().getUniformLocation("cameraPosition");
		m_cameraLook = m_material->getShader().getUniformLocation("cameraLook");
	}


	void URenderBlock::vertexAttribPointer(GLuint shaderProgram, std::string name, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer)
	{
		GLint location = glGetAttribLocation(shaderProgram, name.c_str());
		LogGLErrors();

		if (location >= 0)
		{
			glVertexAttribPointer(location, size, type, normalized, stride, pointer);
			LogGLErrors();
		}
	}

	GLint URenderBlock::enableVertexAttribArray(GLuint shaderProgram, std::string name)
	{
		GLint location = glGetAttribLocation(shaderProgram, name.c_str());
		LogGLErrors();

		if (location >= 0)
		{
			glEnableVertexAttribArray(location);
			LogGLErrors();
		}

		return location;
	}

	std::vector<uint64> dumpedAnimData;

	GLuint URenderBlock::m_animationBuffer = 0;
	int URenderBlock::m_animationSize = 0;

	void URenderBlock::dumpAnimationData()
	{
		// temporary hack
		if (std::find(dumpedAnimData.begin(), dumpedAnimData.end(), m_material->getShader().UUID.getID()) != dumpedAnimData.end())
		{
			return;
		}

		dumpedAnimData.push_back(m_material->getShader().UUID.getID());

		// hack to be refactored 
		if (m_animationBuffer && m_mesh->m_animationsRef != nullptr)
		{
			m_material->getShader().bindProgram();
			m_material->getShader().bindShaderStorageBuffer("Animations", m_animationBuffer);
			m_material->getShader().setUniform1i("animationCount", m_animationSize);
			return;
		}

		if (m_mesh == nullptr)
		{
			LogWarning("Mesh not assigned!");
			return;
		}

		std::vector<AnimationData> animations;

		if (m_mesh->m_animationsRef != nullptr)
		{
			if (m_mesh->m_animationsRef->size() > 0)
			{
				int count = 0;

				// Populate animations
				for (SharedPtr<UAnimation> anim : *m_mesh->m_animationsRef)
				{
					animations.resize(anim->getBoneAnimations().size());

					if (count > 0)
					{
						continue;
					}

					// Needs updating to support multiple animations
					for (SharedPtr<UBoneAnim> boneAnim : anim->getBoneAnimations())
					{
						animations[count].Duration = anim->getDuration();
						animations[count].TicksPerSecond = anim->getTicksPerSecond();
						animations[count].BoneId = boneAnim->getId();

						for (auto& bone : *m_mesh->m_bones)
						{
							if (bone->getBoneId() == boneAnim->getId())
							{
								//Print("alignment %i", alignof(glm::vec3));
								animations[count].OffsetMatrix = bone->getOffsetMatrix();
								animations[count].ParentBoneId = bone->getParentBoneId();
								break;
							}
						}

						int pCount = 0;

						for (UVectorKey key : boneAnim->getPositionKeys())
						{
							if (pCount >= 256)
							{
								continue;
							}

							//animations[count].PositionKeys[pCount] = key;
							pCount++;
						}

						animations[count].PositionKeyCount = pCount;
						pCount = 0;

						for (UVectorKey key : boneAnim->getScaleKeys())
						{
							if (pCount >= 256)
							{
								continue;
							}

							//animations[count].ScaleKeys[pCount] = key;
							pCount++;
						}

						animations[count].ScaleKeyCount = pCount;
						pCount = 0;

						for (UQuatKey key : boneAnim->getRotationKeys())
						{
							if (pCount >= 256)
							{
								continue;
							}

							//animations[count].RotationKeys[pCount] = key;
							pCount++;
						}

						animations[count].RotationKeyCount = pCount;
						count++;
					}
				}
			}
		}

		if (!animations.size())
		{
			return;
		}

		if (m_animationBuffer == 0)
		{
			glGenBuffers(1, &m_animationBuffer);
		}

		m_animationSize = animations.size();

		m_material->getShader().bindProgram();
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_animationBuffer);
		glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(AnimationData) * animations.size(), &(animations[0]), GL_STATIC_DRAW);

		m_material->getShader().bindShaderStorageBuffer("Animations", m_animationBuffer);
		m_material->getShader().setUniform1i("animationCount", animations.size());

		//glBindBuffer(GL_TEXTURE_BUFFER, m_animationBuffer);
		//LogGLErrors();
		//glBufferData(GL_TEXTURE_BUFFER, sizeof(AnimationData) * animations.size(), &(animations[0]), GL_STATIC_DRAW);
		//LogGLErrors();
		//
		//if (m_animationBufferTextureId == 0)
		//{
		//	glGenTextures(1, &m_animationBufferTextureId);
		//	glActiveTexture(GL_TEXTURE31);
		//	LogGLErrors();
		//	glBindTexture(GL_TEXTURE_BUFFER, m_animationBufferTextureId);
		//	LogGLErrors();
		//	glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, m_animationBuffer);
		//	LogGLErrors();
		//}

		//m_material->getShader().bindTextureBufferObject("Animations", (uint32)GL_TEXTURE31);


		// Locking mechanism using SSBO's 

		//if (m_animationLockBuffer == 0)
		//{
		//	glGenBuffers(1, &m_animationLockBuffer);
		//}
		//
		//glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_animationLockBuffer);
		//glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(int) * animations.size(), nullptr, GL_DYNAMIC_DRAW);
		////
		//m_material->getShader().bindShaderStorageBuffer("AnimationLocks", m_animationLockBuffer);


		LogGLErrors();
		//glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
		LogGLErrors();
	}

	void URenderBlock::dumpBufferData()
	{
		if (m_mesh == nullptr)
		{
			LogWarning("Mesh not assigned!");
			return;
		}

		setDrawMode(m_mesh->getDrawMode());

		size_t vertexBufferSize = m_mesh->getVertexBufferSize();
		size_t textureBufferSize = m_mesh->getTextureBufferSize();
		size_t texture3DBufferSize = m_mesh->getTexture3dBufferSize();
		size_t colourBufferSize = m_mesh->getColourBufferSize();
		size_t normalBufferSize = m_mesh->getNormalBufferSize();
		size_t tangentBufferSize = m_mesh->getTangentBufferSize();
		size_t biTangentBufferSize = m_mesh->getBiTangentBufferSize();
		size_t weigthSize = m_mesh->getWeightBufferSize();
		size_t boneIdSize = m_mesh->getBoneIdBufferSize();

		glBindBuffer(GL_ARRAY_BUFFER, m_bufferId);

		size_t bufferSize =
			vertexBufferSize +
			textureBufferSize +
			texture3DBufferSize +
			colourBufferSize +
			normalBufferSize +
			tangentBufferSize +
			biTangentBufferSize +
			weigthSize +
			boneIdSize;

		/// Dynamic allocate storage based on the buffers the current mesh is using.
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * ((bufferSize)), 0, GL_DYNAMIC_DRAW);

		/// Dynamically set the buffer data for the buffers that are currently in use.
		size_t bufferOffset = 0;

		if (vertexBufferSize) {
			float* vertices = static_cast<float*>(glm::value_ptr(m_mesh->m_vertices->front()));
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * vertexBufferSize, &vertices[0]);
			bufferOffset += vertexBufferSize;
		}

		if (textureBufferSize) {
			float* textureCoords = static_cast<float*>(glm::value_ptr(m_mesh->m_uvs->front()));
			glBufferSubData(GL_ARRAY_BUFFER, sizeof(float) * bufferOffset,
				sizeof(float) * textureBufferSize, &textureCoords[0]);
			bufferOffset += textureBufferSize;
		}
		else if (texture3DBufferSize)
		{
			float* textureCoords = static_cast<float*>(glm::value_ptr(m_mesh->m_uvs3d->front()));
			glBufferSubData(GL_ARRAY_BUFFER, sizeof(float) * bufferOffset,
				sizeof(float) * texture3DBufferSize, &textureCoords[0]);
			bufferOffset += textureBufferSize;
		}

		if (colourBufferSize) {
			float* vertexColours = static_cast<float*>(glm::value_ptr(m_mesh->m_colours->front()));
			glBufferSubData(GL_ARRAY_BUFFER, sizeof(float) * bufferOffset,
				sizeof(float) * colourBufferSize, &vertexColours[0]);
			bufferOffset += colourBufferSize;
		}

		if (normalBufferSize) {
			float* normals = static_cast<float*>(glm::value_ptr(m_mesh->m_normals->front()));
			glBufferSubData(GL_ARRAY_BUFFER, sizeof(float) * bufferOffset,
				sizeof(float) * normalBufferSize, &normals[0]);
			bufferOffset += normalBufferSize;
		}

		if (tangentBufferSize) {
			float* tangents = static_cast<float*>(glm::value_ptr(m_mesh->getTangents()->front()));
			glBufferSubData(GL_ARRAY_BUFFER, sizeof(float) * bufferOffset,
				sizeof(float) * tangentBufferSize, &tangents[0]);
			bufferOffset += tangentBufferSize;
		}

		if (biTangentBufferSize) {
			float* tangents = static_cast<float*>(glm::value_ptr(m_mesh->getBiTangents()->front()));
			glBufferSubData(GL_ARRAY_BUFFER, sizeof(float) * bufferOffset,
				sizeof(float) * biTangentBufferSize, &tangents[0]);
			bufferOffset += biTangentBufferSize;
		}

		if (weigthSize) {
			float* weights = static_cast<float*>(glm::value_ptr(m_mesh->getBoneWeights()->front()));
			glBufferSubData(GL_ARRAY_BUFFER, sizeof(float) * bufferOffset,
				sizeof(float) * weigthSize, &weights[0]);
			bufferOffset += weigthSize;
		}

		if (boneIdSize) {
			float* boneids = static_cast<float*>(glm::value_ptr(m_mesh->getBoneIds()->front()));
			glBufferSubData(GL_ARRAY_BUFFER, sizeof(float) * bufferOffset,
				sizeof(float) * boneIdSize, &boneids[0]);
			bufferOffset += boneIdSize;
		}

		LogGLErrors();
		setVertexAttributes();
	}
}
#endif
