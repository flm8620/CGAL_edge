#ifndef _PROBING_
#define _PROBING_
#include <vector>
#include <random>
#include <limits>
#include <memory>
#include <list>
#include <cassert>

extern int face_id_count;
extern int vertex_id_count;
extern int edge_id_count;

//Kernel : Kernel Concept
//Tds : TriangulationDataStructure_2
template<class Kernel, class Tds>
struct MyEdge : std::enable_shared_from_this<MyEdge<Kernel, Tds>> {
    typedef typename Tds::Face_handle Face_handle;
    typedef typename Tds::Vertex_handle Vertex_handle;
    typedef MyEdge<Kernel, Tds> Self;
    int id;
    Vertex_handle v_start;
    Vertex_handle v_end;
private:
    void register_to_vertex() {
        if (v_start != Vertex_handle() && v_end != Vertex_handle()) {
            std::weak_ptr<Self> weak = this->shared_from_this();
            v_start->add_edge(weak);
            v_end->add_edge(weak);
        }
    }

public:
    MyEdge(Vertex_handle v0, Vertex_handle v1) :v_start(v0), v_end(v1) {
        if (v0 != Vertex_handle() && v1 != Vertex_handle()) {
            if (v0 == v1) {
                throw "loop edge";
            }
        }
        id = edge_id_count++;
    }
    static std::shared_ptr<Self> make_shared_ptr(Vertex_handle v0 = Vertex_handle(), Vertex_handle v1 = Vertex_handle()) {
        auto ptr = std::make_shared<Self>(v0, v1);
        ptr->register_to_vertex();
        return ptr;
    }

    MyEdge(const MyEdge& other) = delete;

    bool is_empty() {
        return v_start != Vertex_handle() && v_end != Vertex_handle();
    }
};

// Kernel : Kernel Concept
// Vbb : Triangulation_vertex_base_2< Traits, Vb > Class
// MyVertex : TriangulationDataStructure_2::Vertex Concept 
template <class Kernel, class Vbb>
class MyVertex : public Vbb
{

public:
    typedef Vbb Base;
    typedef typename Base::Vertex_handle Vertex_handle;
    typedef typename Kernel::Point_2 Point;
    typedef typename Base::Triangulation_data_structure Tds;
    typedef MyEdge<Kernel, Tds> MyEdge;
    template < typename TDS2 >
    struct Rebind_TDS {
        typedef typename Base::template Rebind_TDS<TDS2>::Other Vb2;
        typedef MyVertex<Kernel, Vb2> Other;
    };
private:
    int m_vid;
    std::list<std::weak_ptr<MyEdge>> m_edge_weak_ptrs;
public:
    MyVertex() : Base() { m_vid = vertex_id_count++; }
    MyVertex(const Point & p, void* f) : Base(p, f) { m_vid = vertex_id_count++; }
    MyVertex(const Point & p) : Base(p) { m_vid = vertex_id_count++; }
    int id() { return m_vid; }

    void add_edge(std::weak_ptr<MyEdge> e) {
        for (auto ptr : m_edge_weak_ptrs) {
            if (!ptr.owner_before(e) && !e.owner_before(ptr)) {//equals
                throw "error";
            }
        }
        m_edge_weak_ptrs.push_back(e);
    }
    std::shared_ptr<MyEdge> get_edge_to_another_vertex(Vertex_handle v) {
        std::vector<std::shared_ptr<MyEdge>> ptrs;
        auto it = m_edge_weak_ptrs.begin();
        while (it != m_edge_weak_ptrs.end()) {
            if (it->expired()) {
                it = m_edge_weak_ptrs.erase(it);
            }
            else {
                std::shared_ptr<MyEdge> shared = it->lock();
                if (shared->v_start == v || shared->v_end == v) {
                    ptrs.push_back(shared);
                }
                it++;
            }
        }
        if (ptrs.empty())
            return nullptr;
        else if (ptrs.size() != 1) {
            throw "repeated edge";
        }
        else {
            return ptrs.front();
        }
    }

};



// Kernel : Kernel Concept
// Fbb : Triangulation_face_base_2< Traits, Fb > Class
// MyFaceBase : TriangulationDataStructure_2::Face Concept 
template <class Kernel, class Fbb>
class MyFaceBase : public Fbb
{
public:
    typedef Fbb Base;
    typedef typename Base::Vertex_handle Vertex_handle;
    typedef typename Base::Face_handle Face_handle;
    typedef typename Base::Triangulation_data_structure Tds;
    typedef MyEdge<Kernel, Tds> MyEdge;
    template < typename TDS2 >
    struct Rebind_TDS {
        typedef typename Base::template Rebind_TDS<TDS2>::Other Fb2;
        typedef MyFaceBase<Kernel, Fb2> Other;
    };

private:
    int m_fid;
    std::shared_ptr<MyEdge> m_edge_ptrs[3];
public:
    MyFaceBase() //: Base() don't do this, there are no virtual function in base
    {
        m_fid = face_id_count++;
        this->set_vertices();
        Base::set_neighbors();
    }
    MyFaceBase(const MyFaceBase& other) :Base(other) {
        for (int i : {0, 1, 2}) {
            assert(other.m_edge_ptrs[i]);
            m_edge_ptrs[i] = other.m_edge_ptrs[i]; // share edge
        }
        m_fid = face_id_count++;
    }
    MyFaceBase(Vertex_handle v0,
        Vertex_handle v1,
        Vertex_handle v2)
        //: Base(v0, v1, v2) don't do this, there are no virtual function in base
    {
        m_fid = face_id_count++;
        this->set_vertices(v0, v1, v2);
        Base::set_neighbors();
    }
    MyFaceBase(Vertex_handle v0, Vertex_handle v1, Vertex_handle v2,
        Face_handle f0, Face_handle f1, Face_handle f2)
        //: Base(v0, v1, v2, f0, f1, f2) don't do this, there are no virtual function in base
    {
        m_fid = face_id_count++;
        this->set_vertices(v0, v1, v2);
        Base::set_neighbors(f0, f1, f2);
    }
    int id() const { return m_fid; }
    void set_vertex(int i, Vertex_handle v) {
        Base::set_vertex(i, v);
        for (int j : {this->ccw(i), this->cw(i)}) {
            Vertex_handle vj = this->vertex(j);
            std::shared_ptr<MyEdge> existing_edge_ptr;
            if (vj != Vertex_handle()) {
                if (vj != v) {
                    existing_edge_ptr = vj->get_edge_to_another_vertex(v);
                }
                else {//this new edge is a loop
                    existing_edge_ptr = nullptr;
                }
            }
            else {
                existing_edge_ptr = nullptr;
            }
            if (existing_edge_ptr != nullptr) {//this edge already exist, point to it
                m_edge_ptrs[3 - i - j] = existing_edge_ptr;
            }
            else {//create a new edge
                if (vj != v) {
                    m_edge_ptrs[3 - i - j] = MyEdge::make_shared_ptr(vj, v);
                }
                else {//this new edge is a loop
                    m_edge_ptrs[3 - i - j] = MyEdge::make_shared_ptr();
                }
            }
        }
    }
    void set_vertices() {
        Base::set_vertices();
        for (int i : {0, 1, 2}) m_edge_ptrs[i] = MyEdge::make_shared_ptr();
    }
    void set_vertices(Vertex_handle v0, Vertex_handle v1, Vertex_handle v2) {
        //Base::set_vertices(v0, v1, v2);  don't do this, there are no virtual function in base
        this->set_vertex(0, v0);
        this->set_vertex(1, v1);
        this->set_vertex(2, v2);
    }
    void reorient()
    {
        //exchange the vertices 0 and 1
        Vertex_handle v[3];
        for (int i : {0, 1, 2})
            v[i] = this->vertex(i);
        Face_handle n[3];
        for (int i : {0, 1, 2})
            n[i] = this->neighbor(i);
        this->set_vertices(v[1], v[0], v[2]);
        this->set_neighbors(n[1], n[0], n[2]);
    }
    void ccw_permute()
    {
        Vertex_handle v[3];
        for (int i : {0, 1, 2})
            v[i] = this->vertex(i);
        Face_handle n[3];
        for (int i : {0, 1, 2})
            n[i] = this->neighbor(i);
        set_vertices(v[2], v[0], v[1]);
        set_neighbors(n[2], n[0], n[1]);
    }
    void cw_permute()
    {
        Vertex_handle v[3];
        for (int i : {0, 1, 2})
            v[i] = this->vertex(i);
        Face_handle n[3];
        for (int i : {0, 1, 2})
            n[i] = this->neighbor(i);
        set_vertices(v[1], v[2], v[0]);
        set_neighbors(n[1], n[2], n[0]);
    }

    std::shared_ptr<MyEdge> get_edge_ptr(int i) {
        return m_edge_ptrs[i];
    }
};

#endif