#include "probing.h"
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/IO/Color.h>
#include <CGAL/Triangulation_2.h>
#include <CGAL/Triangulation_face_base_with_info_2.h>
typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef K::Point_2 Point;

#include <CGAL/Delaunay_triangulation_2.h>
template <class Kernel>
using Vb = CGAL::Triangulation_vertex_base_2<Kernel>;

template <class Kernel>
using Fb = CGAL::Triangulation_face_base_2<Kernel>;

template <class Kernel>
using MVb = MyVertex<Kernel, Vb<Kernel>>;

template <class Kernel>
using MFb = MyFaceBase<Kernel, Fb<Kernel>>;

template <class Kernel>
using TDS = CGAL::Triangulation_data_structure_2<MVb<Kernel>, MFb<Kernel>>;

template <class Kernel>
using Triangulation = CGAL::Triangulation_2<Kernel, TDS<Kernel>>;

void print_all(Triangulation<K>& t) {
    typedef typename Triangulation<K>::All_faces_iterator All_faces_iterator;
    typedef typename Triangulation<K>::Face_handle Face_handle;
    typedef typename Triangulation<K>::Vertex_handle Vertex_handle;
    std::cout << "---------------print all-------------------" << std::endl;
    All_faces_iterator f;
    Face_handle fh;
    for (f = t.all_faces_begin(); f != t.all_faces_end(); f++) {
        Face_handle fh = f;
        std::cout << "Face " << fh->id() << (t.is_infinite(fh) ? " infinite" : " finite") << std::endl;
        for (int i : {0, 1, 2}) {
            Vertex_handle v = fh->vertex(i);
            std::cout << "\tVertex " << v->id() << (t.is_infinite(v) ? " infinite" : " finite") << "\t";
            if (!t.is_infinite(v))
                std::cout << "(" << v->point().x() << ", " << v->point().y() << ")";
            std::cout << std::endl;
        }
        for (int i : {0, 1, 2}) {
            auto edge_ptr = fh->get_edge_ptr(i);
            assert(edge_ptr);
            std::cout << "\tEdge " << edge_ptr->id << " ";
            for (auto v : { edge_ptr->v_start,edge_ptr->v_end }) {
                if (v != Vertex_handle())
                    std::cout << " v" << v->id();
                else
                    std::cout << " vNull";
            }
            std::cout << std::endl;
        }
    }
}

int main() {
    Triangulation<K> t;
    t.insert(Point(0, 1));
    t.insert(Point(0, 0));
    t.insert(Point(2, 0));
    print_all(t);
    t.insert(Point(2, 2));
    print_all(t);
    return 0;
}