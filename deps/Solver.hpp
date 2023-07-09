#ifndef SOLVER_HPP
# define SOLVER_HPP

# include "rubik.hpp"
# include "Cube.hpp"
# include <iomanip>
# include <memory>
# include <unistd.h>
# include <queue>
# include <unordered_set>
# include <chrono>
#include <fstream>
#include <thread>

# define PTR std::shared_ptr<Cube>
# define MPTR(y) std::make_shared<Cube>(y)


class Solver
{
	public:
		map<int64_t, string> phaseHash[4];
		char moves[6] = {'F','R','U','B','L','D'};
		int phase = 1;

		bool allowedMoves[18];
		unordered_set<int> middleSlice = {fr, fl, bl, br};
		int64_t phaseGoal[5];
		unordered_set<int64_t> ids;
		int64_t (Solver::*idPhase[4])(Cube) = {
			&Solver::idPhase1,
			&Solver::idPhase2,
			&Solver::idPhase3,
			&Solver::idPhase4
		};


		int64_t	idPhase1(Cube c){
			int64_t id = 0; 
			for (int i = 0; i < 12; i++)
			{
				id <<= 1;
				id += c.eOri[i];
			}
			return id;
		}
		int64_t	idPhase2(Cube c){
			int64_t id = 0; 
			for (int i = 0; i < 8; i++)
			{
				id <<= 2;
				id += c.cOri[i];
			}
			for (int i = 0; i < 12; i++){
				id <<= 2;
				if (c.ePos[i] < 8)
					id++;
			}
			return id;
		}
		int64_t	idPhase3(Cube c){
			string faces = "FRUBLD";

			int64_t id = 0; 
			for (int i = 0; i < 7; i++){
				for (int j = 0; j < 3; j++){
					id <<= 1;
					char t = c.cornerNames[c.cPos[i]][(c.cOri[i] + j) % 3];
					if (!(t == c.cornerNames[i][j] ||
						t == faces[(faces.find(c.cornerNames[i][j]) + 3) % 6]))
						id++;
				}			
			}
			for (int i = 0; i < 11; i++){
				for (int j = 0; j < 2; j++){
					id <<= 1;
					char t = c.edgeNames[c.ePos[i]][(c.eOri[i] + j) % 2];
					if (!(t == c.edgeNames[i][j] ||
						t == faces[(faces.find(c.edgeNames[i][j]) + 3) % 6]))
						id++;
				}			
			}
			for (int i = 0; i < 8; i++)
			{
				id <<= 1;
				if (c.cPos[i] % 4 != i % 4)
					id++;
			}
			id <<= 1;
			for (int i = 0; i < 8; i++ )
				for( int j = i + 1; j < 8; j++ )
					id ^= c.cPos[i] > c.cPos[j];
			return id;
		}
		int64_t	idPhase4(Cube c){
			string faces = "FRUBLD";
			
			int64_t id = 0; 
			for (int i = 0; i < 8; i++){
				for (int j = 0; j < 3; j++){
					id <<= 1;
					char t = c.cornerNames[c.cPos[i]][(c.cOri[i] + j) % 3];
					if (t == faces[(faces.find(c.cornerNames[i][j]) + 3) % 6])
						id++;
				}			
			}
			for (int i = 0; i < 12; i++){
				for (int j = 0; j < 2; j++){
					id <<= 1;
					char t = c.edgeNames[c.ePos[i]][(c.eOri[i] + j) % 2];
					if (t == faces[(faces.find(c.edgeNames[i][j]) + 3) % 6])
						id++;
				}			
			}
			return id;
		}
		Cube	BFS(int step, queue<Cube> level)
		{
			if (step == 0){
				ids.clear();
				int64_t num = getPhaseId(level.front(), phase);
				if (num == phaseGoal[phase]){
					nextPhase();
					return level.front();
				}
			}
			queue<Cube> next;
			Cube cur;
			while (!level.empty()){
				cur = level.front();
				level.pop();
				int count = 0;
				for (int move = 0; move < 6; move++)
				{
					for (int amount = 0; amount < 3; amount++)
					{
						cur.rotCube(moves[move], 1);
						int64_t id;
						if (allowedMoves[count] == 1){
							id = getPhaseId(cur, phase);
							if (ids.find(id) == ids.end())
							{
								cur.path += moves[move];
								cur.path += (amount + '1');
								if (id == phaseGoal[phase])
								{
									nextPhase();
									return cur;
								}
								ids.insert(id);
								next.push(cur);
								cur.path = cur.path.substr(0, cur.path.length() - 2);
							}
						}
						count++;
					}
					cur.rotCube(moves[move], 1);
				}
			}
			if (next.empty())
			{
				cout << RED << "Solution not found\n";
				exit(0);
			}
			return BFS(step + 1, next);
		}
		int64_t 	getPhaseId(Cube c, int phase)
		{
			int64_t id = 0;
			
			id = (*this.*(idPhase[phase - 1]))(c);
			return id;
		}
		void	nextPhase(){
			switch (phase){
				case 1:
					allowedMoves[0] = 0;
					allowedMoves[2] = 0;
					allowedMoves[9] = 0;
					allowedMoves[11] = 0;
					break;
				case 2:
					allowedMoves[3] = 0;
					allowedMoves[5] = 0;
					allowedMoves[12] = 0;
					allowedMoves[14] = 0;
					break;
				case 3:
					allowedMoves[6] = 0;
					allowedMoves[8] = 0;
					allowedMoves[15] = 0;
					allowedMoves[17] = 0;
			}
			phase++;
		}
		Solver(Cube c) {
			Cube tmp;

			for (int i = 0; i < 18; i++)
				allowedMoves[i] = 1;
			for (int i = 1 ; i <= 4; i++)
				phaseGoal[i] = getPhaseId(tmp, i);
		}
		void	readData(std::string file, int phase)
		{
			ifstream input(file);
			if (input.fail())
			{
				cout << RED << "File " << file << " not found >.<" << endl;
				exit(1);
			}
			int64_t hash;
			string moves;
			while (input >> hash >> moves)
				phaseHash[phase - 1][hash] = moves;
		}
		void	printBoard(PTR b);
};

#endif