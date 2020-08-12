#include "TrieEventManager.h"
#include "BlipSubscriber.h"
#include "BlipSystem.h"

TrieEventNode::TrieEventNode() {
    sons = new LinkedList< blip::Pair<char, TrieEventNode*> >();
}

TrieEventNode* TrieEventNode::getSon(char c) {
    TrieEventNode* res = nullptr;

    for (auto node = sons->getFirst(); node; node = node->next) {
        if (node->data.f == c) {
            res = node->data.s;
            break;
        }
    }

    return res;
}

TrieEventNode::~TrieEventNode() {
    for (auto node = sons->getFirst(); node; node = node->next) {
        delete node->data.s;
    }
    delete sons;
    delete subscribers;
}



TrieEventManager::TrieEventManager() {
    root = new TrieEventNode;
}

bool TrieEventManager::find(const String& event) {
    TrieEventNode* node = root;
    for (int i = 0; i < event.length(); ++i) {
        TrieEventNode* next = node->getSon(event[i]);
        if (next == nullptr) {
            return false;
        }
        node = next;
    }

    if (node->subscribers->size() > 0) {
        return true;
    }
    return false;
}

void TrieEventManager::add(BlipSubscriber* sub, const String& event) {
    TrieEventNode* node = root;
    for (int i = 0; i < event.length(); ++i) {
        TrieEventNode* next = node->getSon(event[i]);
        if (next == nullptr) {
            next = new TrieEventNode;
            node->sons->add({ event[i], next });
        }
        node = next;
    }

    if (node->subscribers == nullptr) {
        node->subscribers = new LinkedList< BlipSubscriber* >();
    }

    node->subscribers->add(sub);
}

void TrieEventManager::remove(BlipSubscriber* sub, const String& event) {
    TrieEventNode* node = root;
    for (int i = 0; i < event.length(); ++i) {
        TrieEventNode* next = node->getSon(event[i]);
        if (next == nullptr) {
            return;
        }
        node = next;
    }

    if (node->subscribers != nullptr) {
        node->subscribers->removeByValue(sub);
    }
}

void TrieEventManager::notify(BlipSystem* system, const String& event) {
    TrieEventNode* node = root;
    for (int i = 0; i < event.length(); ++i) {
        TrieEventNode* next = node->getSon(event[i]);
        if (next == nullptr) {
            return;
        }
        node = next;
    }

    LinkedListNode<BlipSubscriber*>* nextLink;
    for (auto link = node->subscribers->getFirst(); link; link = nextLink) {
        nextLink = link->next;
        link->data->update(system, event);
    }
}

TrieEventManager::~TrieEventManager() {
    delete root;
}
