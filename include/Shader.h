#pragma once

#include <cstdint>
#include <fstream>
#include <sstream>
#include <string>
#include <string_view>
#include <GL/glew.h>
//#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>


#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <vector>

class Shader
{
public:
	// the program ID
	uint32_t ID;

	// constructor reads and builds the shader
	static Shader fromRawData(const std::string& vertexShaderData, const std::string& fragmentShaderData);
	static Shader fromFile(const std::string& vertexShaderData, const std::string& fragmentShaderData);
	

	void use() const;

	void setBool(std::string_view name, bool value) const;
	void setInt(std::string_view name, int value) const;
	void setFloat(std::string_view name, float value) const;
	void setFloatArray(std::string_view name, const std::vector<float>& data) const;
	void setVec2(std::string_view name, const glm::vec2& value) const;
	void setVec2(std::string_view name, float x, float y) const;
	void setVec3(std::string_view name, const glm::vec3& value) const;
	void setVec3(std::string_view name, float x, float y, float z) const;
	void setVec4(std::string_view name, const glm::vec4& value) const;
	void setVec4(std::string_view name, float x, float y, float z, float w);
	void setMat2(std::string_view name, const glm::mat2& mat) const;
	void setMat3(std::string_view name, const glm::mat3& mat) const;
	void setMat4(std::string_view name, const glm::mat4& value) const;

private:
	Shader() = default;

	void checkCompileErrors(unsigned int shader, std::string type);

	Shader(const std::string& vertexShaderData, const std::string& fragmentShaderData);

	uint32_t compileVertexShader(const std::string& vertexShaderData);
	uint32_t compileFragmentShader(const std::string& fragmentShaderPath);
	void linkShader(uint32_t vertexShader, uint32_t fragmentShader);

	[[nodiscard]] std::string getFile(std::string_view path) const;

};