/**
 * Copyright (c) 2012 Philippe Proulx <philippe.proulx@polymtl.ca>
 *
 * This file is part of libdelorean.
 *
 * libdelorean is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libdelorean is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with libdelorean.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <fstream>
#include <ostream>
#include <cassert>
#include <tr1/memory>
#include <cstdlib>
#include <sstream>

#include <delorean/InHistory.hpp>
#include <delorean/intervals/AbstractInterval.hpp>
#include <delorean/HistoryConfig.hpp>
#include <delorean/CoreNode.hpp>
#include <delorean/LeafNode.hpp>
#include <delorean/ex/IOEx.hpp>
#include <delorean/ex/UnknownNodeTypeEx.hpp>
#include <delorean/FixedConfig.hpp>


using namespace std;
using namespace std::tr1;

InHistory::InHistory()
: AbstractHistory() {
}

InHistory::InHistory(HistoryConfig config)
: AbstractHistory(config) {
}

/**
 * Opens a history tree file for reading only.
 * The file must exist and be of the correct format.
 *
 * @throw IOEx if no file, incorrect format, or already open
 *
 */
void InHistory::open() {
    // is this history tree already opened?
    if (this->_opened) {
        throw IOEx("This tree is already opened");
    }

    // open stream
    this->_stream.open(this->_config._stateFile.c_str(), fstream::in | fstream::binary);

    // check for open errors
    if (!this->_stream) {
        throw IOEx("Unable to open file");
    }

    // unserialize tree header
    try{
        this->unserializeHeader();
    }catch(InvalidFormatEx& ex){
        this->_stream.close();
        throw;
    }catch(IOEx& ex){
        this->_stream.close();
        throw;
    }

    // store latest branch in memory
    this->buildLatestBranch();

    // The user could not possibly know the start and end times of the tree
    // Set it to the correct value using the root node
    _config._treeStart = _latest_branch[0]->getStart();
    _end = _latest_branch[0]->getEnd();

    // update internal status
    this->_opened = true;
}

/**
 * From an existing tree on the disk, rebuild the latest branch.
 *
 */
void InHistory::buildLatestBranch(void) {
    assert(this->_node_count > 0);

    this->_latest_branch.clear();

    //Read root
    AbstractNode::SharedPtr node = createNodeFromSeq(this->_root_seq);
    CoreNode::SharedPtr coreNode = dynamic_pointer_cast<CoreNode>(node);
    node->reopen();
    _latest_branch.push_back(node);

    //Follow the latest branch down
    while(coreNode != NULL && coreNode->getNbChildren() > 0) {
        unsigned int nbChildren = coreNode->getNbChildren();
        seq_number_t nextSeq = coreNode->getChild(nbChildren-1);
        node = createNodeFromSeq(nextSeq);
        node->reopen();
        this->_latest_branch.push_back(node);
        coreNode = dynamic_pointer_cast<CoreNode>(node);
    }
    return;
}

void InHistory::unserializeHeader(void) {
    fstream& f = this->_stream;
    f.exceptions ( fstream::failbit | fstream::badbit );
    try{
        // goto beginning
        f.seekg(0);

        // verify that this is an history tree file
        int32_t mn;
        f.read((char*) &mn, sizeof(int32_t));
        if (mn != HF_MAGIC_NUMBER) {
            f.exceptions(fstream::goodbit);
            ostringstream oss;
            oss << "Wrong start Bytes. Expected : " << HF_MAGIC_NUMBER << " Got : " << mn;
            throw InvalidFormatEx(oss.str());
        }

        // file version
        int32_t major, minor;
        f.read((char*) &major, sizeof(int32_t));
        f.read((char*) &minor, sizeof(int32_t));
        if (major != HF_MAJOR || minor != HF_MINOR) {
            f.exceptions(fstream::goodbit);
            throw InvalidFormatEx("Unsupported version");
        }

        // block size
        f.read((char*) &this->_config._blockSize, sizeof(int32_t));

        // max. children
        f.read((char*) &this->_config._maxChildren, sizeof(int32_t));

        // node count
        f.read((char*) &this->_node_count, sizeof(int32_t));

        // root sequence number
        f.read((char*) &this->_root_seq, sizeof(int32_t));
    }catch(fstream::failure& e) {
        f.clear();
        f.exceptions(fstream::goodbit);
        throw IOEx("Error while reading file header");
    }
    f.exceptions(fstream::goodbit);
}

void InHistory::close(timestamp_t end) {
    // is this history tree at least opened?
    if (!this->_opened) {
        throw IOEx("This tree was not open");
    }

    // close stream
    this->_stream.close();

    // update internal status
    this->_opened = false;
}

InHistory::~InHistory() {
    if (this->_opened) {
        this->close();
    }
}

/**
 * Inner method to select the next child of the current node intersecting
 * the given timestamp. Useful for moving down the tree following one
 * branch.
 *
 * @param currentNode
 * @param t
 * @return The child node intersecting t
 */
AbstractNode::SharedPtr InHistory::selectNextChild(CoreNode::SharedPtr currentNode, timestamp_t timestamp) const {
    assert ( currentNode->getNbChildren() > 0 );
    int potentialNextSeqNb = currentNode->getChildAtTimestamp(timestamp);

    /* Once we exit this loop, we should have found a children to follow.
     * If we didn't, there's a problem. */
    assert ( potentialNextSeqNb != currentNode->getSequenceNumber() );

    /* Since this code path is quite performance-critical, avoid iterating
     * through the whole latestBranch array if we know for sure the next
     * node has to be on disk */
    if ( currentNode->isDone() ) {
        return createNodeFromSeq(potentialNextSeqNb);
    } else {
        AbstractNode::SharedPtr childNode;
        childNode = fetchNodeFromLatestBranch(potentialNextSeqNb);
        if(childNode == 0){
            childNode = createNodeFromSeq(potentialNextSeqNb);
        }
        return childNode;
    }
}

vector<AbstractInterval::SharedPtr> InHistory::query(timestamp_t timestamp) const {
    if ( !checkValidTime(timestamp) ) {
        throw TimeRangeEx("Query timestamp outside of bounds");
    }

    // We start by reading the information in the root node
    AbstractNode::SharedPtr currentNode = _latest_branch[0];
    vector<AbstractInterval::SharedPtr> relevantIntervals;
    currentNode->writeInfoFromNode(relevantIntervals, timestamp);


    // Then we follow the branch down in the relevant children
    // Stop at leaf nodes or if a core node has no children
    while ( nodeHasChildren(currentNode) ) {
        CoreNode::SharedPtr coreNode = dynamic_pointer_cast<CoreNode>(currentNode);
        assert(coreNode != NULL);
        currentNode = selectNextChild(coreNode, timestamp);
        currentNode->writeInfoFromNode(relevantIntervals, timestamp);
    }

    // The relevantIntervals should now be filled with everything needed
    return relevantIntervals;
}

AbstractInterval::SharedPtr InHistory::query(timestamp_t timestamp, attribute_t key) const {
    if ( !checkValidTime(timestamp) ) {
        throw TimeRangeEx("Query timestamp outside of bounds");
    }

    AbstractNode::SharedPtr currentNode = _latest_branch[0];
    AbstractInterval::SharedPtr interval = currentNode->getRelevantInterval(timestamp, key);

    // Follow the branch down until we find the required interval or there are no children left
    // Stop at leaf nodes or if a core node has no children
    while ( interval == NULL && nodeHasChildren(currentNode) ) {
        CoreNode::SharedPtr coreNode = dynamic_pointer_cast<CoreNode>(currentNode);
        currentNode = selectNextChild(coreNode, timestamp);
        interval = currentNode->getRelevantInterval(timestamp, key);
    }

    // The interval should now contain the relevant interval
    return interval;
}

std::multimap<attribute_t, AbstractInterval::SharedPtr> InHistory::sparseQuery(timestamp_t timestamp) const {
    if ( !checkValidTime(timestamp) ) {
        throw TimeRangeEx("Query timestamp outside of bounds");
    }

    // We start by reading the information in the root node
    AbstractNode::SharedPtr currentNode = _latest_branch[0];
    multimap<attribute_t, AbstractInterval::SharedPtr> relevantIntervals;
    currentNode->writeInfoFromNode(relevantIntervals, timestamp);


    // Then we follow the branch down in the relevant children
    // Stop at leaf nodes or if a core node has no children
    while ( nodeHasChildren(currentNode) ) {
        CoreNode::SharedPtr coreNode = dynamic_pointer_cast<CoreNode>(currentNode);
        assert(coreNode != NULL);
        currentNode = selectNextChild(coreNode, timestamp);
        currentNode->writeInfoFromNode(relevantIntervals, timestamp);
    }

    // The relevantIntervals should now be filled with everything needed
    return relevantIntervals;
}

AbstractNode::SharedPtr InHistory::createNodeFromStream() const {
    fstream& f = this->_stream;
    streampos init_pos = f.tellg();

    // node to return
    AbstractNode::SharedPtr n;

    // type
    node_type_t nt;
    f.read((char*) &nt, sizeof(node_type_t));

    // create node according to type
    switch (nt) {
        case NT_CORE:
        n.reset(new CoreNode(this->_config));
        break;

        case NT_LEAF:
        n.reset(new LeafNode(this->_config));
        break;

        default:
        throw(UnknownNodeTypeEx(nt));
        break;
    }

    // unserialize the node
    f.seekg(init_pos);
    n->unserialize(f, this->_ic);

    return n;
}

AbstractNode::SharedPtr InHistory::createNodeFromSeq(seq_number_t seq) const {
    // make sure everything is okay
    assert((unsigned int) seq < this->_node_count);

    // compute where the node begins in file
    streampos offset = filePosFromSeq(seq);

    // seek there
    this->_stream.seekg(offset, ios::beg);

    // get the node
    return this->createNodeFromStream();
}

AbstractNode::SharedPtr InHistory::fetchNodeFromLatestBranch(seq_number_t seq) const {

    std::vector<AbstractNode::SharedPtr>::const_iterator it;

    for (it = _latest_branch.begin(); it != _latest_branch.end(); it++) {
        if ((*it)->getSequenceNumber() == seq) {
            return *it;
        }
    }
    return AbstractNode::SharedPtr();
}

void InHistory::test(void) {
    for (unsigned int i = 0; i < this->_node_count; ++i) {
        AbstractNode::SharedPtr node(fetchNodeFromLatestBranch(i));
        //The node is not in the latest branch, it must be on disk
        if (node == 0)
            node = this->createNodeFromSeq(i);
        cout << *node << endl;
    }
}
