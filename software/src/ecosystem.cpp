#include <iterator>

#include <common.hpp>
#include <ecosystem.hpp>

#define COIN_FLIP 2
#define QUARTER_KILL 0.25

using namespace std;

Ecosystem::Ecosystem(int num_species, int num_nets){
  #if DEBUG
    cout << "[INFO][ECOSYSTEM]: Entered consturctor." << endl;
  #endif

  for(int index = 0; index < num_species; index++){
    organisms.push_back(new Species());
  }
  for(list<Species *>::iterator it = organisms.begin(); it != organisms.end(); it++){
    for(int index = 0; index < num_nets; index++)
      (*it)->add_network(new Network());
  }

  #if DEBUG
    cout << "[INFO][ECOSYSTEM]: Exiting consturctor." << endl;
  #endif
}

Ecosystem::~Ecosystem(){
  #if DEBUG
    cout << "[INFO][ECOSYSTEM]: Entered de-consturctor." << endl;
  #endif

  for(list<Species *>::iterator it = organisms.begin(); it != organisms.end(); it++){
    if(*it != NULL){
      delete *it;
      *it = NULL;
    }
  }
  organisms.clear();

  for(list<Network *>::iterator it = strongest.begin(); it != strongest.end(); it++){
    if(*it != NULL){
      delete *it;
      *it = NULL;
    }
  }
  strongest.clear();

  #if DEBUG
    cout << "[INFO][ECOSYSTEM]: Exiting de-consturctor." << endl;
  #endif
}

bool Ecosystem::live(){
  #if DEBUG
    cout << "[INFO][ECOSYSTEM]: Entered live." << endl;
  #endif

  int num_networks = 0;
  int size_of_nets = 0;

  Network *child;

  list<Network * >::iterator itr;
  list<Network * >::iterator next_itr;
  
  /* Run Individual networks of every species */
  for(list<Species *>::iterator species_iter= organisms.begin(); species_iter!= organisms.end(); ++species_iter){
    for(list<Network *>::iterator network_iter = (*species_iter)->get_networks()->begin(); network_iter != (*species_iter)->get_networks()->end(); ++network_iter){
      (*network_iter)->run(); 
      (*network_iter)->calculate_fit();

      num_networks++;
    }

    if((*species_iter)->is_stale())
      (*species_iter)->mutate();

    (*species_iter)->get_networks()->sort(compareNets);

    size_of_nets = (*species_iter)->get_networks()->size();
    itr = (*species_iter)->get_networks()->begin();
    for(int index = 0; index < size_of_nets; index++){
      next_itr = itr;

      if(++next_itr != (*species_iter)->get_networks()->end()){
        child = breed(*itr, *next_itr);  
        (*species_iter)->add_network(child);
      }

      itr++;
    }

    kill_unfit((*species_iter)->get_networks());

    strongest.push_back((*species_iter)->get_fittest_net());
  }

  /* Figure out if you need to make a new species */
  Species *newSpecies = new Species();

  for(list<Species *>::iterator it = organisms.begin(); it != organisms.end(); it++){
    for(list<Network *>::iterator itr = (*it)->get_networks()->begin(); itr != (*it)->get_networks()->end(); itr++){      
      if((*itr)->get_compatibility_distance() > 0.25){
        if((*it)->get_networks()->size() > 0)
          (*it)->get_networks()->splice(itr, *(newSpecies->get_networks()));
      }

      #if DEBUG
        cout << "Compatibility number: " << (*itr)->get_compatibility_distance() << endl;
      #endif
    }
  }

  if(newSpecies->get_networks()->size() > 0){
    organisms.push_back(newSpecies);
  }
  else{
    if(newSpecies != NULL){
      delete newSpecies;
      newSpecies = NULL;
    }
  }  


  /* Simple statistics displayed after each run */
  int counter = 1, numNodes = 0;
  for(list<Species *>::iterator species_iter= organisms.begin(); species_iter!= organisms.end(); ++species_iter){
    #if DEBUG
      cout << "Number of networks for species #" << counter << " : " << (*species_iter)->get_networks()->size() << endl;
    #endif
    
    for(list<Network *>::iterator itr = (*species_iter)->get_networks()->begin(); itr != (*species_iter)->get_networks()->end(); itr++){
      numNodes += (*itr)->get_num_nodes();
    }
    
    #if DEBUG
      cout << "\tTotal number of nodes: " << numNodes << endl;
    #endif
    
    numNodes = 0;
    counter++;
  }

  #if DEBUG
    cout << "[INFO][ECOSYSTEM]: Exiting live." << endl;
  #endif

  return true;
}

bool Ecosystem::kill_unfit(list<Network *> *unfit){
  #if DEBUG
    cout << "[INFO][ECOSYSTEM]: Entered kill_unfit." << endl;
  #endif

  if(unfit->size() > 0)
    unfit->sort(compareNets);
  else{
    #if DEBUG
      cout << "[INFO][ECOSYSTEM]: Exiting kill_unfit." << endl;
    #endif
    return false;
  }

  int size = unfit->size() * QUARTER_KILL;

  list<Network *>::iterator from = unfit->begin();
  list<Network *>::iterator to = unfit->begin();

  for(int index = 0; index < size; index++){
    to++;
  }
  
  /* Killing the unfit from the list */
  for(list<Network *>::iterator it = from; it != to; it++){
    if(*it != NULL){
      delete *it;
      *it = NULL;
    } 
  }

  /* Resize the list to exclude the NULL Pointers */
  list<Network *> temp;
  for(list<Network *>::iterator it = to; it != unfit->end(); it++){
    if(*it != NULL)
      temp.push_back(*it);
  }

  *unfit = temp;

  #if DEBUG
    cout << "[INFO][ECOSYSTEM]: Exiting kill_unfit." << endl;
  #endif

  return true;
}

bool Ecosystem::compareGenes(const Gene *one, const Gene *two){
  #if DEBUG
    cout << "[INFO][ECOSYSTEM]: Entered compareGenes." << endl;
    cout << "[INFO][ECOSYSTEM]: Exiting compareGenes." << endl;
  #endif

  return one->get_inov_id() < two->get_inov_id();
}

bool Ecosystem::compareNets(const Network *one, const Network *two){ // sorts less fit first to more fit later.
  #if DEBUG
    cout << "[INFO][ECOSYSTEM]: Entered compareNets." << endl;
    cout << "[INFO][ECOSYSTEM]: Exiting compareNets." << endl;
  #endif

  return one->get_fitness() < two->get_fitness();
}

Network* Ecosystem::breed(Network *one, Network *two){
  #if DEBUG
    cout << "[INFO][ECOSYSTEM]: Entered breed." << endl;
  #endif

  Network *newNet = new Network();

  if(one == NULL || two == NULL){
    #if DEBUG
      cout << "[INFO][ECOSYSTEM]: Exiting breed." << endl;
    #endif
    return newNet;
  }


  if(one->get_genes()->size() > 1 && two->get_genes()->size() > 1){
    one->get_genes()->sort(compareGenes);
    two->get_genes()->sort(compareGenes);
  }
  else{
    #if DEBUG
      cout << "[INFO][ECOSYSTEM]: Exiting breed." << endl;
    #endif
    return newNet;
  }

  list<Gene *>::iterator itone = one->get_genes()->begin();
  list<Gene *>::iterator ittwo = two->get_genes()->begin();

  int randNum;

  while(itone != one->get_genes()->end() && ittwo != two->get_genes()->end()){
    randNum = rand() % COIN_FLIP;

    if(randNum)
      newNet->add_gene((*ittwo)->get_input_node(), (*ittwo)->get_output_node());
    else newNet->add_gene((*itone)->get_input_node(), (*itone)->get_output_node());

    itone++;
    ittwo++;
  }

  #if DEBUG
    cout << "[INFO][ECOSYSTEM]: Exiting breed." << endl;
  #endif

  return newNet;
}

list<Species *>* Ecosystem::get_organisms(){
  #if DEBUG
    cout << "[INFO][ECOSYSTEM]: Entered get_organisms." << endl;
    cout << "[INFO][ECOSYSTEM]: Exiting get_organisms." << endl;
  #endif

  return &organisms;
}