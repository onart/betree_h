#include "behaviortree.h"
#include <iostream>

struct __exampleContext
{
    int doorDegree = 0;
    bool haveKey = 0;
    bool isDoorLocked = 1;
};


int main(){
    {
        using namespace onart;
        Sequence* tr = new Sequence {
            new Fallback {
                new Actor2([](void* ctx){
                    auto realctx = reinterpret_cast<__exampleContext*>(ctx);
                    return realctx->doorDegree >= 45 ? NodeState::SUCCESS : NodeState::FAILURE;
                }),
                new Actor2([](void* ctx){                        
                    auto realctx = reinterpret_cast<__exampleContext*>(ctx);
                    if(realctx->isDoorLocked) return NodeState::FAILURE;
                    if(realctx->doorDegree < 45) realctx->doorDegree++;
                    return realctx->doorDegree >= 45 ? NodeState::SUCCESS : NodeState::RUNNING;
                }),
                new Sequence {
                    new Actor2([](void* ctx){
                        auto realctx = reinterpret_cast<__exampleContext*>(ctx);
                        return realctx->haveKey ? NodeState::SUCCESS : NodeState::FAILURE;
                    }),
                    new Actor2([](void* ctx){
                        auto realctx = reinterpret_cast<__exampleContext*>(ctx);
                        realctx->isDoorLocked = false;
                        return NodeState::SUCCESS;
                    }),
                    new Actor2([](void* ctx){
                        auto realctx = reinterpret_cast<__exampleContext*>(ctx);
                        if(realctx->isDoorLocked) return NodeState::FAILURE;
                        if(realctx->doorDegree < 45) realctx->doorDegree++;
                        return realctx->doorDegree >= 45 ? NodeState::SUCCESS : NodeState::RUNNING;
                    })
                    },
                new Actor2([](void* ctx){
                    std::cout << "breaking the door...\n";
                    return NodeState::SUCCESS;
                })
            },
            new Actor2([](void* ctx){
                std::cout << "enter room...\n";
                return NodeState::SUCCESS;
            })
        };
        __exampleContext ctx;
        tr->tick(&ctx);
        tr->free();
        delete tr;
    }
}