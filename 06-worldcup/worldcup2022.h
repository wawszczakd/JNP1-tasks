#ifndef WORLDCUP_2022_H
#define WORLDCUP_2022_H

#include <list>
#include <vector>
#include "worldcup.h"

/**
 * Implementation of the WorldCup interface.
 */
class WorldCup2022 : public WorldCup {
private:
  static const uint32_t STARTING_MONEY = 1000;
  static const uint32_t MAX_PLAYERS = 11;
  static const uint32_t MIN_PLAYERS = 2;
  static const uint32_t NUM_OF_SQUARES = 12;
  static const uint32_t MAX_DICE = 2;
  static const uint32_t MIN_DICE = 2;

  class Dice : public Die {
    public:
      [[nodiscard]] unsigned short roll() const override {
        unsigned short rolled = 0;
        for (auto d : dice_list)
          rolled += d->roll();
        return rolled;
      }

      std::list<std::shared_ptr<Die>> dice_list;
  };

  class Player {
    public:
      Player(std::string new_name) : name(new_name), alive(true) {
        turns_to_wait = 0;
        money = STARTING_MONEY;
        square_num = 0;
      }

      const std::string name;
      uint32_t money;
      uint32_t turns_to_wait;
      uint32_t square_num;

      bool isAlive() const {
        return alive;
      }

      void die() {
        alive = false;
      }

      const std::string & getStatus() {
        if (!alive)
          status = "*** bankrut ***";
        else if (turns_to_wait == 0)
          status = "w grze";
        else
          status = "*** czekanie: " + std::to_string(turns_to_wait) + " ***";

        return status;
      }

    private:
      std::string status;
      bool alive;
  };

  /**
   * Interface for all board squares.
   */
  class BoardSquare {
    public:
      BoardSquare(const std::string & sq_name) : name(sq_name) {}

      virtual void land(Player & player) = 0;

      virtual void passBy(Player & player) = 0;

      const std::string name;
  };

  class Board {
    public:
      Board() : squares(NUM_OF_SQUARES) {
        /**
         * Setting up the board squares.
         */
        std::shared_ptr<SeasonStartSquare>
          start = std::make_shared<SeasonStartSquare>("Początek sezonu");
        std::shared_ptr<MatchSquare>
          sm_match = std::make_shared<MatchSquare>("Mecz z San Marino", 160, 1),
          li_match = std::make_shared<MatchSquare>("Mecz z Lichtensteinem", 220,
                                                   1),
          mx_match = std::make_shared<MatchSquare>("Mecz z Meksykiem", 300,
                                                   2.5),
          as_match = std::make_shared<MatchSquare>("Mecz z Arabią Saudyjską",
                                                   280, 2.5),
          ar_match = std::make_shared<MatchSquare>("Mecz z Argentyną", 250,
                                                   2.5),
          fr_match = std::make_shared<MatchSquare>("Mecz z Francją", 400, 4);
        std::shared_ptr<FreeSquare>
          free_sq = std::make_shared<FreeSquare>("Dzień wolny od treningu");
        std::shared_ptr<YellowCardSquare>
          yellow = std::make_shared<YellowCardSquare>("Żółta kartka", 3);
        std::shared_ptr<BettingSquare>
          bet_sq = std::make_shared<BettingSquare>("Bukmacher", 100);
        std::shared_ptr<GoalSquare>
          goal_sq = std::make_shared<GoalSquare>("Gol", 120);
        std::shared_ptr<PenaltySquare>
          pen_sq = std::make_shared<PenaltySquare>("Rzut karny", 180);

        squares[0] = start;
        squares[1] = sm_match;
        squares[2] = free_sq;
        squares[3] = li_match;
        squares[4] = yellow;
        squares[5] = mx_match;
        squares[6] = as_match;
        squares[7] = bet_sq;
        squares[8] = ar_match;
        squares[9] = goal_sq;
        squares[10] = fr_match;
        squares[11] = pen_sq;
      }

      std::vector<std::shared_ptr<BoardSquare>> squares;
  };

  class FreeSquare : public BoardSquare {
    public:
      FreeSquare(const std::string & sq_name) : BoardSquare(sq_name) {}

      void land(Player & player) override {
        (void)player;
      }

      void passBy(Player & player) override {
        (void)player;
      }
  };

  class SeasonStartSquare : public BoardSquare {
    public:
      SeasonStartSquare(const std::string & sq_name) : BoardSquare(sq_name) {}

      void land(Player & player) override {
        if (player.isAlive())
          player.money += 50;
      }

      void passBy(Player & player) override {
        if (player.isAlive())
          player.money += 50;
      }
  };

  class GoalSquare : public BoardSquare {
    public:
      GoalSquare(const std::string & sq_name, uint32_t bonus_amount) :
          BoardSquare(sq_name), bonus(bonus_amount) {}

      void land(Player & player) override {
        if (player.isAlive())
          player.money += bonus;
      }

      void passBy(Player & player) override {
        (void)player;
      }

    private:
      const uint32_t bonus;
  };

  class PenaltySquare : public BoardSquare {
    public:
      PenaltySquare(const std::string & sq_name, uint32_t pen_amount) :
          BoardSquare(sq_name), penalty(pen_amount) {}

      void land(Player & player) override {
        if (player.isAlive()) {
          if(player.money < penalty){
            player.money = 0;
            player.die();
          } else
            player.money -= penalty;
        }
      }

      void passBy(Player & player) override {
        (void)player;
      }

    private:
      uint32_t penalty;
  };

  class BettingSquare : public BoardSquare {
    public:
      BettingSquare(const std::string & sq_name, uint32_t wager_amount) :
          BoardSquare(sq_name), counter(0), wager(wager_amount) {}

      void land(Player & player) override {
        if (!player.isAlive())
          return;
        if (counter == 0)
          player.money += wager;
        else if(player.money < wager){
          player.money = 0;
          player.die();
        } else
          player.money -= wager;
        counter = (counter + 1) % 3;
      }

      void passBy(Player & player) override {
        (void)player;
      }

    private:
      uint32_t counter;
      uint32_t wager;
  };

  class YellowCardSquare : public BoardSquare {
    public:
      YellowCardSquare(const std::string & sq_name, uint32_t wait) :
          BoardSquare(sq_name), waiting_time(wait) {}

      void land(Player & player) override {
        player.turns_to_wait += waiting_time;
      }
      void passBy(Player & player) override {
        (void)player;
      }

    private:
      uint32_t waiting_time;
  };

  class MatchSquare : public BoardSquare {
    public:
      MatchSquare(const std::string & sq_name, uint32_t match_fee,
                  double match_weight) :
      BoardSquare(sq_name), fee(match_fee), weight(match_weight), pool(0) {}

      void land(Player & player) override {
        if (player.isAlive()) {
          player.money += pool * weight;
          pool = 0;
        }
      }

      void passBy(Player & player) override {
        if (player.isAlive()) {
          if(player.money < fee){
            player.die();
            pool += player.money;
            player.money = 0;
          } else{
            player.money -= fee;
            pool += fee;
          }
        }
      }

    private:
      uint32_t fee;
      double weight;
      uint32_t pool;
  };

  uint32_t round_number;
  Dice dice;
  std::shared_ptr<ScoreBoard> scoreboard;
  std::list<Player> players;
  Board game_board;

  void checkDataCorrectness() const {
    if (dice.dice_list.size() > MAX_DICE)
      throw TooManyDiceException();
    if (dice.dice_list.size() < MIN_DICE)
      throw TooFewDiceException();
    if (players.size() > MAX_PLAYERS)
      throw TooManyPlayersException();
    if (players.size() < MIN_PLAYERS)
      throw TooFewPlayersException();
  }

public:
  WorldCup2022() {
    scoreboard = nullptr; ///< Set default scoreboard that does nothing
    round_number = 0;
  }

  void addDie(std::shared_ptr<Die> die) {
    if (die != nullptr)
      dice.dice_list.push_back(die);
  }

  void addPlayer(std::string const &name) {
    Player new_player(name);
    players.push_back(new_player);
  }

  void setScoreBoard(std::shared_ptr<ScoreBoard> scoreboard) {
    this->scoreboard  = scoreboard;
  }

  void play(unsigned int max_rounds) {
    checkDataCorrectness();

    uint32_t players_alive = players.size(), num_rolled;
    for (uint32_t round = 0; round < max_rounds && players_alive > 1; round++) {
      if(scoreboard != nullptr)
        scoreboard->onRound(round_number);
      for (Player & player : players) {
        if (!player.isAlive()) ///< Skip bankrupt players.
          continue;
        if(players_alive == 1)
          break;

        if (player.turns_to_wait == 0) {
          num_rolled = dice.roll();
          if(num_rolled == 0) ///< Deals with rolling zero.
            game_board.squares.at(player.square_num)->land(player);
          while (num_rolled > 0) {
            player.square_num = (player.square_num + 1) % NUM_OF_SQUARES;

            if (num_rolled == 1) {
              game_board.squares.at(player.square_num)->land(player);
            }
            else {
              game_board.squares.at(player.square_num)->passBy(player);
            }

            num_rolled--;
          }
        }

        if(scoreboard != nullptr)
          scoreboard->onTurn(player.name, player.getStatus(),
                           game_board.squares.at(player.square_num)->name,
                           player.money);
        if (player.turns_to_wait > 0) ///< The player has one less turn to wait.
          player.turns_to_wait--;
        if (!player.isAlive())
          players_alive--;
      }
      round_number++;
    }
    
    if(players_alive > 0){ ///< There is a winner unless everybody has 0 money.
      uint32_t max_money = 0;
      Player* winner = nullptr;
      for(Player & player : players){
        if(player.money > max_money){
          winner = &player;
          max_money = player.money;  
        }
      }
      if(winner != nullptr)
        scoreboard->onWin(winner->name);
    }
  }

  class TooManyDiceException : public std::exception {
    public:
      const char *what() const noexcept override {
        return "Too many dice.";
      }
  };

  class TooFewDiceException : public std::exception {
    public:
      const char *what() const noexcept override {
        return "Too few dice.";
      }
  };

  class TooManyPlayersException : public std::exception {
    public:
      const char *what() const noexcept override {
        return "Too many players.";
      }
  };

  class TooFewPlayersException : public std::exception {
    public:
      const char *what() const noexcept override {
        return "Too few players.";
      }
  };
};

#endif
