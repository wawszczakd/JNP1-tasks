/**
 * @file Języki i narzędzia programowania I, rozwiązanie zadania 1 (TOP7)
 * @authors Piotr Trzaskowski, Roman Radionov, Dominik Wawszczak
 * @date 16.10.2022
 */

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <regex>

using umap = std::unordered_map<uint32_t, size_t>;
using uset = std::unordered_set<uint32_t>;
using pii = std::pair<uint32_t, size_t>;
using pis = std::pair<uint32_t, std::string>;
using std::vector;
using std::string;
using std::cin;
using std::cout;
using std::cerr;
using std::getline;
using std::regex;
using std::sregex_iterator;

namespace
{
	constexpr size_t MAX_NUMBER = 99'999'999;
	
	/**
	 * Numer utworu, liczba zdobytych punktów we wszystkich zakończonych
	 * notowaniach.
	 */
	umap top;
	/**
	 * Ostatnie głosowanie top. Utwór o numerze `last_top[i]` był na pozycji
	 * `i + 1` w poprzedmim rankingu.
	 */
	uint32_t last_top[7] = {0, 0, 0, 0, 0, 0, 0};
	
	/**
	 * Maksymalny numer utworu, na który można głosować w aktualnej nocie.
	 */
	size_t MAX = 0;
	/**
	 * Numer utworu, liczba oddanych na niego głosów.
	 */
	umap note;
	/**
	 * Ostatnia zakończona nota. Utwór o numerze `last_note[i]` był na pozycji
	 * `i + 1` w poprzedmim rankingu.
	 */
	uint32_t last_note[7] = {0, 0, 0, 0, 0, 0, 0};
	/**
	 * Lista uwórów, które wypadły z głosowania.
	 */
	uset dropped_from_vote;
	
	/**
	 * Dodaje głosy do notowania na podane utwory.
	 */
	void add_votes(const uset& votes)
	{
		for (uint32_t vote : votes)
		{
			if (note.find(vote) == note.end())
				note[vote] = 1;
			else
				note[vote] = note[vote] + 1;
		}
	}
	
	/**
	 * Dodaje zawartość `last_note` do mapy `top` z odpowiednią liczbą punktów.
	 */
	void add_last_note_to_top()
	{
		for (size_t i = 0; i < 7; i++)
		{
			if (last_note[i] == 0)
				continue;
			
			if (top.find(last_note[i]) == top.end())
				top[last_note[i]] = 7 - i;
			else
				top[last_note[i]] = top[last_note[i]] + 7 - i;
		}
	}
	
	/**
	 * Tworzy listę top7 dla zadanej mapy. Zwraca wektor par (numer utworu,
	 * liczba głosów). Wszystkie dane muszą być poprawne. Lista zawiera
	 * maksymalnie 7 utworów.
	 */
	vector<pii> make_top_7(umap vote_data)
	{
		vector<pii> top_7;
		
		for (size_t i = 0; i < 7; i++)
		{
			pii max_vote = {MAX_NUMBER + 1, 0};
			for (const pii p : vote_data)
			{
				if (p.second > max_vote.second)
					max_vote = p;
				else if (p.second == max_vote.second && p.first < max_vote.first)
					max_vote = p;
			}
			vote_data.erase(max_vote.first);
			if (max_vote.first <= MAX)
				top_7.push_back(max_vote);
		}
		
		return top_7;
	}
	
	/**
	 * Zamyka dotychczasowe notowanie i rozpoczyna nowe.
	 */
	vector<pis> new_note(size_t new_MAX)
	{
		vector<pis> top_node_list;
		
		vector<pii> top_7 = make_top_7(note);
		
		/**
		 * Dodawanie utwórów, które wypadły z głosowania do `dropped_from_vote`.
		 */
		for (uint32_t i : last_note)
		{
			if (i != 0)
			{
				bool dropped = true;
				for (const pii vote : top_7)
					if (i == vote.first)
						dropped = false;
				if (dropped)
					dropped_from_vote.insert(i);
			}
		}
		
		/**
		 * Tworzenie `top_node_list`.
		 */
		size_t counter = 0;
		for (const pii p : top_7)
		{
			string new_position = "-";
			for (size_t i = 0; i < 7; i++)
			{
				if (p.first == last_note[i])
				{
					if (i < counter)
						new_position += '0' + counter - i;
					else
						new_position = '0' + i - counter;
				}
			}
			
			top_node_list.emplace_back(p.first, new_position);
			counter++;
		}
		
		/**
		 * Aktualizowanie listy `last_note`.
		 */
		counter = 0;
		for (const pis& vote : top_node_list)
		{
			last_note[counter] = vote.first;
			counter++;
		}
		while (counter < 7)
		{
			last_note[counter] = 0;
			counter++;
		}
		
		/**
		 * Czyszczenie noty.
		 */
		note.clear();
		
		/**
		 * Dodawanie notowania do TOP.
		 */
		add_last_note_to_top();
		
		MAX = new_MAX;
		
		return top_node_list;
	}
	
	/**
	 * Zwraca wektor z maksymalnie 7 parami. Para zawiera numer utworu oraz
	 * liczbę pozycji, o którą zmienił się w rankingu.
	 */
	vector<pis> get_top()
	{
		vector<pii> top_7 = make_top_7(top);
		/**
		 * Czyszczenie mapy `top`, kasując z niej elementy, które nie są w
		 * `top_7` oraz wypadły z głosowania. Po zakończeniu czyszczona mapa
		 * będzie miała co najwyżej 14 elementów.
		 */
		vector<uint32_t> dumpster;
		for (const pii p : top)
			if (dropped_from_vote.find(p.first) != dropped_from_vote.end())
				dumpster.push_back(p.first);
		
		for (uint32_t song : dumpster)
			top.erase(song);
		
		/**
		 * Przywrócenie `top7` do `top`.
		 */
		for (const pii p : top_7)
			top[p.first] = p.second;
		
		/**
		 * Tworzenie `top_list`.
		 */
		vector<pis> top_list;
		size_t counter = 0;
		
		for (const pii p : top_7)
		{
			string new_position = "-";
			for (size_t i = 0; i < 7; i++)
			{
				if (p.first == last_top[i])
				{
					if (i < counter)
						new_position += '0' + counter - i;
					else
						new_position = '0' + i - counter;
				}
			}
			
			top_list.emplace_back(p.first, new_position);
			counter++;
		}
		
		/**
		 * Aktualizowanie listy `last_top`.
		 */
		counter = 0;
		for (const pis& vote : top_list)
		{
			last_top[counter] = vote.first;
			counter++;
		}
		while (counter < 7)
		{
			last_top[counter] = 0;
			counter++;
		}
		
		return top_list;
	}
	
	static regex const TOP("\\s*TOP\\s*");
	static regex const NEW_MAX("\\s*NEW\\s+[1-9][0-9]{0,7}\\s*");
	static regex const VOTE("\\s*[1-9][0-9]{0,7}(\\s+[1-9][0-9]{0,7})*\\s*");
	static regex const EMPTY_LINE("\\s*");
	static regex const WORD_REGEX("([^\\s]+)");
	
	/**
	 * Wypisuje top i notowanie.
	 */
	void print(const vector<pis>& p)
	{
		for (const pis& pa : p)
			cout << pa.first << ' ' << pa.second << '\n';
	}
	
	void print_error(uint32_t line_number, string input)
	{
		cerr << "Error in line " << line_number << ": " << input << '\n';
	}
	
	/**
	 * Przekształca głos z napisu w set numerów piosenek.
	 */
	uset get_votes(const string& input, size_t& num)
	{
		uset votes;
		uint32_t vote = 0;
		auto word_iter = sregex_iterator(input.begin(), input.end(), WORD_REGEX);
		while (word_iter != sregex_iterator())
		{
			vote = stoi((word_iter++)->str());
			if (vote <= MAX && dropped_from_vote.find(vote) == dropped_from_vote.end())
				votes.insert(vote);
			vote = 0;
			num++;
		}
		
		return votes;
	}
	
	void read_input()
	{
		string input;
		uint32_t line_number = 0, new_MAX = 0;
		size_t num = 0;
		uset votes;
		
		while (getline(cin, input))
		{
			line_number++;
			
			if (regex_match(input, TOP))
				print(get_top());
			else if (regex_match(input, NEW_MAX))
			{
				auto word_iter = sregex_iterator(input.begin(), input.end(), WORD_REGEX);
				word_iter++;
				new_MAX = stoi((word_iter)->str()); 
				if (MAX <= new_MAX)
					print(new_note(new_MAX));
				else
					print_error(line_number, input);
			}
			else if (regex_match(input, VOTE))
			{
				if (MAX == 0)
					print_error(line_number, input);
				else
				{
					num = 0;
					votes = get_votes(input, num);
					if (votes.size() == num)
						add_votes(votes);
					else
						print_error(line_number, input);
				}
			}
			else if (!regex_match(input, EMPTY_LINE))
				print_error(line_number, input);
		}
	}
}

int main()
{
	read_input();
	return 0;
}
