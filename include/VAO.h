#ifndef VAO_H
#define VAO_H

#include <cstddef>

class VAO {
  public:
	VAO() = default;
	~VAO();

	// Copie interdite : sinon deux objets supprimeraient le même Id
	VAO(const VAO &) = delete;
	VAO &operator=(const VAO &) = delete;
	VAO(VAO &&other) noexcept;
	VAO &operator=(VAO &&other) noexcept;

  public:
	bool load();  // glGenVertexArrays
	void bind() const;
	static void unbind();

	// Attribut de type float. Le VBO source doit être bindé avant l'appel.
	void setAttribute(unsigned int index, int size, int stride,
	                  std::size_t offset) const;

	unsigned int id() const { return Id; }

  private:
	void release();
	unsigned int Id = 0;
};

#endif  // VAO_H
