#include "ShaderProgram.h"
#include "../../Internal/Log.h"

namespace s3dge
{
	namespace Graphics
	{
		ShaderProgram::ShaderProgram(cstring vertex, cstring fragment)
			: _vertex(vertex), _fragment(fragment)
		{
			_programID = Load();
			this->Enable();
			this->SetUniform1iv("textureArray", 16, _textureIDs);
		}

		ShaderProgram::~ShaderProgram()
		{
			glDeleteProgram(_programID);
		}

		uint ShaderProgram::Load()
		{
			// Create and compile vertex shader.
			int vertexShader = glCreateShader(GL_VERTEX_SHADER);
			std::string vertexSource = FileUtils::ReadFromFile(_vertex).c_str();
			if (vertexSource == "")
			{
				LOG_ERROR("Cannot find vertex shader source");
				return -1;
			}
			cstring vSource = vertexSource.c_str();
			glShaderSource(vertexShader, 1, &vSource, NULL);
			if (!Compile(vertexShader))
				return -1;

			// Create and compile fragment shader.
			int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
			std::string fragmentSource = FileUtils::ReadFromFile(_fragment).c_str();
			if (fragmentSource == "")
			{
				LOG_ERROR("Cannot find fragment shader source");
				return -1;
			}
			cstring fSource = fragmentSource.c_str();
			glShaderSource(fragmentShader, 1, &fSource, NULL);
			if (!Compile(fragmentShader))
				return -1;

			// Create a shader program and attach compiled shaders to it.
			int program = glCreateProgram();

			glAttachShader(program, vertexShader);
			glAttachShader(program, fragmentShader);

			glLinkProgram(program);
			glValidateProgram(program);

			glDeleteShader(vertexShader);
			glDeleteShader(fragmentShader);

			return program;
		}

		bool ShaderProgram::Compile(int shader)
		{
			// Compile the shader
			glCompileShader(shader);

			// Check the compilation status and report any errors
			int shaderStatus;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &shaderStatus);

			// If the shader failed to compile, display the info log and quit out
			if (shaderStatus == GL_FALSE)
			{
				GLint infoLogLength;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

				char *strInfoLog = new char[infoLogLength + 1];
				glGetShaderInfoLog(shader, infoLogLength, 0, strInfoLog);

				LOG_ERROR("shader compilation failed: ", strInfoLog);
				delete[] strInfoLog;
				glDeleteShader(shader);
				return false;
			}

			return true;
		}

		void ShaderProgram::SetProjection(Maths::mat4 ortho)
		{
			this->Enable();
			this->SetUniformMat4fv("pr_matrix", ortho);
		}

		void ShaderProgram::Enable()
		{
			glUseProgram(_programID);
		}

		void ShaderProgram::Disable()
		{
			glUseProgram(0);
		}

		void ShaderProgram::SetUniformMat4fv(cstring name, const Maths::mat4& matrix)
		{
			glUniformMatrix4fv(glGetUniformLocation(_programID, name), 1, GL_FALSE, matrix.elements);
		}

		void ShaderProgram::SetUniform1f(cstring name, float value)
		{
			glUniform1f(glGetUniformLocation(_programID, name), value);
		}

		void ShaderProgram::SetUniform2f(cstring name, Maths::vec2f value)
		{
			glUniform2f(glGetUniformLocation(_programID, name), value.x, value.y);
		}

		void ShaderProgram::SetUniform3f(cstring name, Maths::vec3f value)
		{
			glUniform3f(glGetUniformLocation(_programID, name), value.x, value.y, value.z);
		}

		void ShaderProgram::SetUniform4f(cstring name, Maths::vec4f value)
		{
			glUniform4f(glGetUniformLocation(_programID, name), value.x, value.y, value.z, value.w);
		}

		void ShaderProgram::SetUniform1i(cstring name, int value)
		{
			glUniform1i(glGetUniformLocation(_programID, name), value);
		}

		void ShaderProgram::SetUniform1iv(cstring name, int count, int* value)
		{
			glUniform1iv(glGetUniformLocation(_programID, name), count, value);
		}
	}
}