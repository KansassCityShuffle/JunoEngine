#ifndef VERTEXUV_H
#define VERTEXUV_H

#include <QVector3D>
#include <QVector2D>

class VertexUV
{
public:
  // Constructors
  Q_DECL_CONSTEXPR VertexUV();
  Q_DECL_CONSTEXPR explicit VertexUV(const QVector3D &position);
  Q_DECL_CONSTEXPR VertexUV(const QVector3D &position, const QVector2D &uv);

  // Accessors / Mutators
  Q_DECL_CONSTEXPR const QVector3D& position() const;
  Q_DECL_CONSTEXPR const QVector2D& uv() const;
  void setPosition(const QVector3D& position);
  void setUV(const QVector2D& uv);

  // OpenGL Helpers
  static const int PositionTupleSize = 3;
  static const int UvTupleSize = 2;
  static Q_DECL_CONSTEXPR int positionOffset();
  static Q_DECL_CONSTEXPR int uvOffset();
  static Q_DECL_CONSTEXPR int stride();

private:
  QVector3D m_position;
  QVector2D m_uv;
};

/*******************************************************************************
 * Inline Implementation
 ******************************************************************************/

// Note: Q_MOVABLE_TYPE means it can be memcpy'd.
Q_DECLARE_TYPEINFO(VertexUV, Q_MOVABLE_TYPE);

// Constructors
Q_DECL_CONSTEXPR inline VertexUV::VertexUV() {}
Q_DECL_CONSTEXPR inline VertexUV::VertexUV(const QVector3D &position) : m_position(position) {}
Q_DECL_CONSTEXPR inline VertexUV::VertexUV(const QVector3D &position, const QVector2D &uv) : m_position(position), m_uv(uv) {}

// Accessors / Mutators
Q_DECL_CONSTEXPR inline const QVector3D& VertexUV::position() const { return m_position; }
Q_DECL_CONSTEXPR inline const QVector2D& VertexUV::uv() const { return m_uv; }
void inline VertexUV::setPosition(const QVector3D& position) { m_position = position; }
void inline VertexUV::setUV(const QVector2D& uv) { m_uv = uv; }

// OpenGL Helpers
Q_DECL_CONSTEXPR inline int VertexUV::positionOffset() { return offsetof(VertexUV, m_position); }
Q_DECL_CONSTEXPR inline int VertexUV::uvOffset() { return offsetof(VertexUV, m_uv); }
Q_DECL_CONSTEXPR inline int VertexUV::stride() { return sizeof(VertexUV); }


#endif // VERTEXUV_H
