#pragma once

#include <GL/glew.h>
#include "libhello/opengl/vertexBuffer.h"
#include "libhello/opengl/indexBuffer.h"
#include <iostream>


template<class vertex_t, class index_t>
class IndexedVertexBuffer : public VertexBuffer<vertex_t>, public IndexBuffer<index_t>{
public:
    typedef VertexBuffer<vertex_t> vbuffer_t;
    typedef IndexBuffer<index_t> ibuffer_t;


    void bind();
    void unbind();

    void bindAndDraw();
    void draw();
    void draw(uint length, void* offset);

    void set(std::vector<vertex_t> &vertices,std::vector<index_t> &indices){
        set(&vertices[0],vertices.size(),&indices[0],indices.size());
    }

    void set(vertex_t* vertices,int vertex_count,index_t* indices,int index_count){
        vbuffer_t::set(vertices,vertex_count);
        ibuffer_t::set(indices,index_count);
    }
};

template<class vertex_t, class index_t>
void IndexedVertexBuffer<vertex_t,index_t>::bindAndDraw(){
    bind();
    draw();
    unbind();
}

template<class vertex_t, class index_t>
void IndexedVertexBuffer<vertex_t,index_t>::draw(){
    glDrawElements( vbuffer_t::draw_mode, ibuffer_t::index_count, GL_UNSIGNED_INT, NULL );
}

template<class vertex_t, class index_t>
void IndexedVertexBuffer<vertex_t,index_t>::draw(uint length, void *offset){
    glDrawElements( vbuffer_t::draw_mode, length, GL_UNSIGNED_INT, offset );
}

template<class vertex_t, class index_t>
void IndexedVertexBuffer<vertex_t,index_t>::bind(){
    vbuffer_t::bind();
    ibuffer_t::bind();
}

template<class vertex_t, class index_t>
void IndexedVertexBuffer<vertex_t,index_t>::unbind(){
    vbuffer_t::unbind();
    ibuffer_t::unbind();
}
