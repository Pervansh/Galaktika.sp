#pragma once

#include "IEventManager.h"
#include <LinkedList.h>
#include <Pair.h>

class BlipSubscriber;
class BlipSystem;

struct TrieEventNode {
    LinkedList< blip::Pair<char, TrieEventNode*> >* sons;
    LinkedList< BlipSubscriber* >* subscribers = nullptr;

    TrieEventNode();

    TrieEventNode* getSon(char c);

    ~TrieEventNode();
};

class TrieEventManager : public IEventManager {
private:
    TrieEventNode* root;

public:
    TrieEventManager();

    bool find(const String& event);
    void add(BlipSubscriber* sub, const String& event);
    void remove(BlipSubscriber* sub, const String& event);
    void notify(BlipSystem* system, const String& event);

    ~TrieEventManager();
};
