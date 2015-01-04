#ifndef OC_SOLVER_H
#define OC_SOLVER_H

#include "contactmanifold.h"
#include <vector>

class Actor;
class Solver
{
public:
    void Solve(const std::vector<ContactManifold>& Manifolds);
private:
    void SolveContact(Actor* A, Actor* B, const Contact& AContact);
    void SolvePosition(Actor* A, Actor* B, const Contact& AContact);
    void SolveVelocity(Actor* A, Actor* B, const Contact& AContact);
};

#endif
