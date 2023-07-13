#ifndef ORGANISM_H
#define ORGANISM_H

#include <optional>
#include <tuple>
#include <cstdint>

template <typename species_t, bool can_eat_meat, bool can_eat_plants>
requires std::equality_comparable<species_t>
class Organism {
    species_t species;
    uint64_t vitality;

    public:
        constexpr Organism(species_t const &sp, uint64_t vital):
                  species(sp), vitality(vital) {};
                  
        constexpr const species_t &get_species() const {
            return species;
        }

        constexpr uint64_t get_vitality() const {
            return vitality;
        }

        constexpr bool is_dead() const {
            return vitality == 0;
        }

        constexpr bool is_plant() const {
            return (can_eat_meat == false && can_eat_plants == false);
        }

        constexpr Organism const mating(Organism const &mate) {
            return Organism<species_t, can_eat_meat, can_eat_plants>
                   (species, (vitality + mate.get_vitality()) / 2);
        }

        template <bool eating_plants>
        constexpr auto fight(Organism<species_t, true, eating_plants>
                                      const &opponent) {
            uint64_t this_vitality = vitality;
            uint64_t opp_vitality = opponent.get_vitality();
            species_t this_species = species;
            species_t opp_species = opponent.get_species();

            if (this_vitality > opp_vitality) {
                this_vitality += opp_vitality / 2;
                opp_vitality = 0;
            } else if (this_vitality == opp_vitality) {
                this_vitality = 0;
                opp_vitality = 0;
            } else {
                opp_vitality += this_vitality / 2;
                this_vitality = 0;
            }

            return std::tuple{
                Organism<species_t, can_eat_meat, can_eat_plants>
                (this_species, this_vitality),

                Organism<species_t, true, eating_plants>
                (opp_species, opp_vitality),

                std::nullopt              
            };
        }

        constexpr auto eat_plant(Organism<species_t, false, false>
                                          const &plant) {
            uint64_t this_vitality = vitality += plant.get_vitality();
            uint64_t plant_vitality = 0;
            species_t this_species = species;
            species_t plant_species = plant.get_species();

            return std::tuple{
                Organism<species_t, can_eat_meat, can_eat_plants>
                (this_species, this_vitality),

                Organism<species_t, false, false>
                (plant_species, plant_vitality),

                std::nullopt
            };
        }

        template <bool eating_meat, bool eating_plants>
        constexpr auto eat(Organism<species_t, eating_meat, eating_plants>
                                    const &being_eaten) {
            uint64_t this_vitality = vitality;
            uint64_t eaten_vitality = being_eaten.get_vitality();
            species_t this_species = species;
            species_t eaten_species = being_eaten.get_species();

            // Zjadamy
            if (this_vitality > eaten_vitality) {
                this_vitality += eaten_vitality / 2;
                eaten_vitality = 0;
            }

            // Jeśli zjadany ma za dużą witalność to nic się nie dzieje.

            return std::tuple{
                Organism<species_t, can_eat_meat, can_eat_plants>
                (this_species, this_vitality),

                Organism<species_t, eating_meat, eating_plants>
                (eaten_species, eaten_vitality),

                std::nullopt
            };
        }
};

template <typename species_t>
using Carnivore = Organism<species_t, true, false>;

template <typename species_t>
using Omnivore = Organism<species_t, true, true>;

template <typename species_t>
using Herbivore = Organism<species_t, false, true>;

template <typename species_t>
using Plant = Organism<species_t, false, false>;


// Funkcje pomocnicze ukryte w namespace details.
namespace details {
    constexpr bool is_plant(std::pair<bool, bool> org) {
        return (!org.first && !org.second);
    }

    constexpr bool can_eat(std::pair<bool, bool> eater, 
                           std::pair<bool, bool> eaten) {
        return (!is_plant(eaten) && eater.first) ||
               (is_plant(eaten) && eater.second);
    }

    template <typename species_t, bool sp1_eats_m, bool sp1_eats_p, 
          typename Head, typename ... Tail>
    constexpr Organism<species_t, sp1_eats_m, sp1_eats_p>
    encounter_series_helper(Organism<species_t, sp1_eats_m, sp1_eats_p> organism1,
                            Head head, Tail ... tail) {

        if constexpr (sizeof...(tail) == 0) {
            return get<0>(encounter(organism1, head));
        } else {
            Organism<species_t, sp1_eats_m, sp1_eats_p> result = 
            get<0>(encounter(organism1, head));

            return encounter_series_helper(result, tail...);
        }

    }
}


template <typename species_t, bool sp1_eats_m, bool sp1_eats_p, 
                              bool sp2_eats_m, bool sp2_eats_p>

    constexpr std::tuple<Organism<species_t, sp1_eats_m, sp1_eats_p>,
                         Organism<species_t, sp2_eats_m, sp2_eats_p>,
                         std::optional<Organism<species_t, sp1_eats_m, sp1_eats_p>>>
    encounter(Organism<species_t, sp1_eats_m, sp1_eats_p> organism1,
              Organism<species_t, sp2_eats_m, sp2_eats_p> organism2) {
        
        // Nawyki żywieniowe obu ogranizmów zapisane w parach dla 
        // przejrzystości i czytelności kodu.
        constexpr std::pair<bool, bool> org1 = {sp1_eats_m, sp1_eats_p};
        constexpr std::pair<bool, bool> org2 = {sp2_eats_m, sp2_eats_p};
               
        // Warunek 1 - Sprawdzany podczas przekazywania argumentów
        // Warunek 2 - Spotkanie roślin
        static_assert(sp1_eats_m || sp1_eats_p || sp2_eats_m || sp2_eats_p); 

        // Warunek 3 - Jedna ze stron martwa
        if (organism1.is_dead() || organism2.is_dead()) {
            return {Organism(organism1), Organism(organism2), std::nullopt};
        }

        // Warunek 4 - Ten sam gatunek
        if (organism1.get_species() == organism2.get_species()) {
            if constexpr (sp1_eats_m == sp2_eats_m && 
                          sp1_eats_p == sp2_eats_p) {
                return {Organism(organism1), Organism(organism2), 
                        organism1.mating(organism2)};
            }
        }

        // Warunek 5 - Spotkanie organizmów które nie mogą się zjeść
        if constexpr (!details::can_eat(org1, org2) &&
                      !details::can_eat(org2, org1)) {
            return {Organism(organism1), Organism(organism2), std::nullopt};
        }
    
        // Warunek 6 - Walka
        else if constexpr (details::can_eat(org1, org2) &&
                           details::can_eat(org2, org1)) {
            return organism1.fight(organism2);
        }

        // Warunek 7 - jedzenie roślin
        else if constexpr ((details::can_eat(org1, org2) &&
                            details::is_plant(org2))) {
            return organism1.eat_plant(organism2);
        }

        else if constexpr ((details::can_eat(org2, org1) &&
                            details::is_plant(org1))) {
            auto result = organism2.eat_plant(organism1);

            return std::tuple{
                get<1>(result),
                get<0>(result),
                get<2>(result)
            };
        }

        // Warunek 8 - jedzenie jednostronne
        else if constexpr (details::can_eat(org1, org2)) {
            return organism1.eat(organism2);
        }

        else if constexpr (details::can_eat(org2, org1)) {
            auto result = organism2.eat(organism1);

            return std::tuple{
                get<1>(result),
                get<0>(result),
                get<2>(result)
            };
        }

        else return {Organism(organism1), Organism(organism2), std::nullopt};
    }


template <typename species_t, bool sp1_eats_m, bool sp1_eats_p,
          typename ... Args>
    constexpr Organism<species_t, sp1_eats_m, sp1_eats_p>
    encounter_series(Organism<species_t, sp1_eats_m, sp1_eats_p> organism1,
                     Args ... args) {
        return details::encounter_series_helper(organism1, args...);
    }


#endif // ORGANISM_H
