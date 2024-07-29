#ifndef __BEHAVIOR_TREE_H__
#define __BEHAVIOR_TREE_H__

#include <vector>
#include <functional>

namespace onart
{

enum class NodeState { 
    SUCCESS = 0,
    FAILURE = 1,
    RUNNING = 2,
};

template<class Context, int id>
NodeState tick(Context& context);

class Node{
    public:
        virtual NodeState tick(void* context) = 0;
        virtual void free() {}
        virtual ~Node() {}
};

template<class Context, int id>
class Actor: public Node{
    public:
        virtual NodeState tick(void* context){
            Context& ctx = *reinterpret_cast<Context*>(context);
            return onart::tick<Context, id>(ctx);
        }
};

class Actor2: public Node{
    public:
        Actor2(std::function<NodeState(void*)> action):action(action){}
        virtual NodeState tick(void* context){ return action(context); }
    private:
        std::function<NodeState(void*)> action;
    
};

class FlowControlNode: public Node{    
    public:
        FlowControlNode(std::initializer_list<Node*> children):children(children){}
        void free(){
            for(Node* child: children) {
                child->free();
                delete child;
            }
        }
    protected:
        std::vector<Node*> children;
        int currentChild = 0;
};

class Sequence: public FlowControlNode{
    public:
        using FlowControlNode::FlowControlNode;
        virtual NodeState tick(void* context){
            for(int i = currentChild; i < children.size(); i++){
                NodeState state = children[i]->tick(context);
                if(state == NodeState::SUCCESS) continue;
                if(state == NodeState::RUNNING) currentChild = i;
                else currentChild = 0;
                return state;
            }
            currentChild = 0;
            return NodeState::SUCCESS;
        }
};

class Fallback: public FlowControlNode{
    public:
        using FlowControlNode::FlowControlNode;
        virtual NodeState tick(void* context){
            for(int i = currentChild; i < children.size(); i++){
                NodeState state = children[i]->tick(context);
                if(state == NodeState::FAILURE) continue;
                if(state == NodeState::RUNNING) currentChild = i;
                else currentChild = 0;
                return state;
            }
            currentChild = 0;
            return NodeState::FAILURE;
        }
};

}

#endif