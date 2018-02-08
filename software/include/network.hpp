#ifndef NETWORK_HPP
#define NETWORK_HPP

/** @brief Class for the Networks in a species.

    This class will hold of the functions that deal
    with networks, and their necessary actions.
    @author Dominguez, Alejandro
    @date Feburary, 2018
    */

#include <iostream>
#include <thread>
#include <list>

#include "node.hpp"
#include "gene.hpp"

using namespace std;

class Network{
   private:
      int fitness;
      int num_nodes;

      float compatibility_distance;

      Node **in_nodes; // the input nodes
      Node **out_nodes; // output nodes
      list<Node *> hidden_nodes; // hidden nodes

      list<Gene *> genes; /**<List for all of the genes associated with the node. */

      thread **threads;
   protected:
      void input_run();
      void hidden_run();
      void output_run();
   public:
      Network();
      ~Network(); // destructor deletes heap memory

      void run();
      void mutate();
      void use_output();
      void set_num_nodes(int);
      void set_compatibility_distance(float);
      void add_gene(Node *, Node *);

      int get_fitness();
      int get_num_nodes();

      bool rand_node();
      bool rand_connection();
      bool add_input_node(int);
      bool add_output_nodes(int);

      Node** get_input();       // names for these can be changed later

      list<Gene *> get_genes();  
};

#endif
