﻿/**
 * Copyright (c) 2017 Darius Rückert
 * Licensed under the MIT License.
 * See LICENSE file for more information.
 */

#include "bulletHelper.h"

namespace Saiga
{
void createCollisionShape(std::vector<Triangle>& mesh, btBvhTriangleMeshShape*& outShape, btTriangleMesh*& outMesh)
{
    if (mesh.size() == 0) return;

    outMesh = new btTriangleMesh();


    for (Triangle& t : mesh)
    {
        outMesh->addTriangle(btVector3(t.a.x, t.a.y, t.a.z), btVector3(t.b.x, t.b.y, t.b.z),
                             btVector3(t.c.x, t.c.y, t.c.z));
    }

    // collision shape
    outShape = new btBvhTriangleMeshShape(outMesh, true);
}

void createConvexCollisionShape(std::vector<Triangle>& mesh, btConvexTriangleMeshShape*& outShape,
                                btTriangleMesh*& outMesh)
{
    if (mesh.size() == 0) return;

    outMesh = new btTriangleMesh();


    for (Triangle& t : mesh)
    {
        outMesh->addTriangle(btVector3(t.a.x, t.a.y, t.a.z), btVector3(t.b.x, t.b.y, t.b.z),
                             btVector3(t.c.x, t.c.y, t.c.z));
    }

    // collision shape
    outShape = new btConvexTriangleMeshShape(outMesh, true);
}

btRigidBody* createRigidBody(btCollisionShape* collisionShape, float mass, vec3 position, quat rotation, float friction)
{
    if (!collisionShape)
    {
        cout << "createRigidbody: collision shape is null!" << endl;
    }

    // rigidbody is dynamic if and only if mass is non zero, otherwise static
    bool isDynamic = (mass != 0.f);

    btVector3 localInertia(0, 0, 0);

    if (isDynamic) collisionShape->calculateLocalInertia(mass, localInertia);


    btDefaultMotionState* motionState = new btDefaultMotionState(btTransform(toBT(rotation), toBT(position)));
    //    MyMotionState* motionState = new MyMotionState(btTransform(q,v),obj);

    btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(mass, motionState, collisionShape, localInertia);

    rigidBodyCI.m_friction = friction;

    btRigidBody* rigidBody = new btRigidBody(rigidBodyCI);

    return rigidBody;
}

void setRigidBodyState(btRigidBody* rigidBody, vec3 position, quat rotation)
{
    btTransform T(toBT(rotation), toBT(position));
    rigidBody->setWorldTransform(T);
    rigidBody->getMotionState()->setWorldTransform(T);
}



}  // namespace Saiga
