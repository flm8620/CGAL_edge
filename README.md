# CGAL_edge
CGAL has only a implicit representation for edge in 2D Triangulation. 
I created a Edge struct and use std::shared_ptr<Edge> in Face_base to point to edge.

The Face-Edge relation is changed only when Face's vertices are modified.
## Edge destruction
When the last incident face of one edge is deleted, this edge is automatically deleted because the last std::shared_ptr<Edge> pointer is deleted.
## Edge sharing & creation
Vertex has weak pointers to incident edges. When a new face is created near an edge, this face will ask its new vertices for edge infomation.
If an edge already exist, this face will get its shared pointer, instead of creating a new duplicated edge. If no edge exists, this face will create a new edge and register its weak pointers to vertices.

# Implementation details
In order to change Face-Edge relation only when Face's vertices are modified, we need to override the following member functions in `CGAL::Triangulation_face_base_2<Kernel>`

```c++
void set_vertex(int i, Vertex_handle v);
void set_vertices();
void set_vertices(Vertex_handle v0, Vertex_handle v1, Vertex_handle v2);
void reorient();
void ccw_permute();
void cw_permute();
```

Attention, these function is not virtual in base class.
