#include "../Libraries/Headers/Artificial/BruteRecurseArtificial.h"
#include "../Libraries/Headers/Artificial/BruteRecurseWithLegalsArtificial.h"
#include "../Libraries/Headers/Artificial/HeuristicSelective/HeuristicSelectiveArtificial.h"
#include "../Libraries/Headers/Artificial/HeuristicSelectiveNoDepth/HeuristicSelectiveArtificialND.h"
#include "../Libraries/Headers/Artificial/PHSelective/PHSelectiveArtificial.h"

#include <iostream>
#include <cstring>
#include <ctime>
#include <chrono>
#include <exception>

using namespace std;

minstd_rand0 randomGenerator(clock());

Artificial* getAi(int type, int depth)
{
	if (type == 0)
	{
		return new BruteRecurseArtificial(depth, &randomGenerator);
	}
	else if (type == 1)
	{
		return new BruteRecurseWithLegalsArtificial(depth, &randomGenerator);
	}
	else if (type == 2)
	{
		return new HeuristicSelectiveArtificialND(100, 1, 50000000, &randomGenerator);
	}
	else if (type == 3)
	{
		FixedUnorderedMap<Board, std::shared_ptr<HeuristicSelectiveGameNode>, BoardHash> nodeRepository(700000);
		FixedUnorderedMap<Board, std::shared_ptr<std::vector<Move const *>>, BoardHash> legalMap(1000000);
		return new HeuristicSelectiveArtificial(100, 50000000, randomGenerator, nodeRepository, legalMap);
	}
	else if (type == 4)
	{
		FixedUnorderedMap<Board, std::shared_ptr<PHSelectiveGameNode>, BoardHash> nodeRepository(700000);
		FixedUnorderedMap<Board, std::shared_ptr<std::vector<Move const *>>, BoardHash> legalMap(1000000);
		return new PHSelectiveArtificial(100, 50000000, randomGenerator, nodeRepository, legalMap);
	}
	else throw exception("Bad AI type");
}

const bool shouldPrintPercent(const int total, const int count)
{
	if (total / 20 == 0)return true;
	return count % (total / 20) == 0;
}

int main()
{
	BitBoardMoveConstants::initialize();
	Logger::registerInfo([](const string& message) {
		cout << message;
	});

moarGamesLabel:

	int ai1Type;
	cout << "\nAI1 Type: ";
	cin >> ai1Type;

	int ai1Depth;
	cout << "\nAI1 Depth: ";
	cin >> ai1Depth;

	int ai2Type;
	cout << "\nAI2 Type: ";
	cin >> ai2Type;

	int ai2Depth;
	cout << "\nAI2 Depth: ";
	cin >> ai2Depth;

	int gameCount;
	cout << "\nGame count: ";
	cin >> gameCount;
	const int GAME_COUNT = gameCount;

	int ai1WhiteWin = 0;
	int ai1WhiteLose = 0;
	int ai1WhiteThreefold = 0;
	int ai1WhiteFiftyMove = 0;
	int ai1WhiteINoLegals = 0;
	int ai1WhiteTheyNoLegals = 0;
	int ai1WhiteMovePlayed = 0;
	double ai1TotalWhiteTime = 0;

	int ai1BlackWin = 0;
	int ai1BlackLose = 0;
	int ai1BlackThreefold = 0;
	int ai1BlackFiftyMove = 0;
	int ai1BlackINoLegals = 0;
	int ai1BlackTheyNoLegals = 0;
	int ai1BlackMovePlayed = 0;
	double ai1TotalBlackTime = 0;

	int ai2WhiteWin = 0;
	int ai2WhiteLose = 0;
	int ai2WhiteThreefold = 0;
	int ai2WhiteFiftyMove = 0;
	int ai2WhiteINoLegals = 0;
	int ai2WhiteTheyNoLegals = 0;
	int ai2WhiteMovePlayed = 0;
	double ai2TotalWhiteTime = 0;

	int ai2BlackWin = 0;
	int ai2BlackLose = 0;
	int ai2BlackThreefold = 0;
	int ai2BlackFiftyMove = 0;
	int ai2BlackINoLegals = 0;
	int ai2BlackTheyNoLegals = 0;
	int ai2BlackMovePlayed = 0;
	double ai2TotalBlackTime = 0;

	while (gameCount > 0)
	{
		const bool ai1IsWhite = gameCount % 2 == 1;
		Artificial* ai1(getAi(ai1Type, ai1Depth));
		Artificial* ai2(getAi(ai2Type, ai2Depth));
		GameSet* gameSet = new GameSet();
		while (gameSet->getStatus() == GameStatus::LIVE)
		{
			Move const * playedMove;
			if (ai1IsWhite == gameSet->isWhiteTurn())
			{
				auto begin = chrono::high_resolution_clock::now();

				playedMove = ai1->getMove(*gameSet);

				auto end = chrono::high_resolution_clock::now();
				if (ai1IsWhite)
				{
					ai1TotalWhiteTime += chrono::duration_cast<chrono::nanoseconds>(end - begin).count();
					++ai1WhiteMovePlayed;
				}
				else
				{
					ai1TotalBlackTime += chrono::duration_cast<chrono::nanoseconds>(end - begin).count();
					++ai1BlackMovePlayed;
				}
			}
			else
			{
				auto begin = chrono::high_resolution_clock::now();

				playedMove = ai2->getMove(*gameSet);

				auto end = chrono::high_resolution_clock::now();
				if (ai1IsWhite)
				{
					ai2TotalBlackTime += chrono::duration_cast<chrono::nanoseconds>(end - begin).count();
					++ai2BlackMovePlayed;
				}
				else
				{
					ai2TotalWhiteTime += chrono::duration_cast<chrono::nanoseconds>(end - begin).count();
					++ai2WhiteMovePlayed;
				}
			}
			GameSet* temp = new GameSet(gameSet->playMove(*playedMove));
			delete gameSet;
			gameSet = temp;
		}
		if (ai1IsWhite)
		{
			if (gameSet->getStatus() == GameStatus::WHITE_WIN)
			{
				++ai1WhiteWin;
				++ai2BlackLose;
			}
			else if (gameSet->getStatus() == GameStatus::BLACK_WIN)
			{
				++ai2BlackWin;
				++ai1WhiteLose;
			}
			else if (gameSet->getStatus() == GameStatus::THREEFOLD_REPETITION)
			{
				++ai1WhiteThreefold;
				++ai2BlackThreefold;
			}
			else if (gameSet->getStatus() == GameStatus::FIFTY_MOVE)
			{
				++ai1WhiteFiftyMove;
				++ai2BlackFiftyMove;
			}
			else
			{
				if (gameSet->isWhiteTurn())
				{
					++ai1WhiteINoLegals;
					++ai2BlackTheyNoLegals;
				}
				else
				{
					++ai1WhiteTheyNoLegals;
					++ai2BlackINoLegals;
				}
			}
		}
		else
		{
			if (gameSet->getStatus() == GameStatus::WHITE_WIN)
			{
				++ai2WhiteWin;
				++ai1BlackLose;
			}
			else if (gameSet->getStatus() == GameStatus::BLACK_WIN)
			{
				++ai1BlackWin;
				++ai2WhiteLose;
			}
			else if (gameSet->getStatus() == GameStatus::THREEFOLD_REPETITION)
			{
				++ai1BlackThreefold;
				++ai2WhiteThreefold;
			}
			else if (gameSet->getStatus() == GameStatus::FIFTY_MOVE)
			{
				++ai1BlackFiftyMove;
				++ai2WhiteFiftyMove;
			}
			else
			{
				if (gameSet->isWhiteTurn())
				{
					++ai1BlackINoLegals;
					++ai2WhiteTheyNoLegals;
				}
				else
				{
					++ai1BlackTheyNoLegals;
					++ai2WhiteINoLegals;
				}
			}
		}
		delete ai1;
		delete ai2;
		delete gameSet;
		--gameCount;
		if (shouldPrintPercent(GAME_COUNT, gameCount))
		{
			cout << (1 - gameCount / (double)GAME_COUNT) * 100 << "%" << endl;
		}
	}

	ai1TotalWhiteTime /= 1000000000.0;
	ai1TotalBlackTime /= 1000000000.0;
	ai2TotalWhiteTime /= 1000000000.0;
	ai2TotalBlackTime /= 1000000000.0;

	cout << "\nAI1 WHITE played: " << (GAME_COUNT + 1) / 2 << endl;
	cout << "AI1 WHITE won: " << ai1WhiteWin << endl;
	cout << "AI1 WHITE lost: " << ai1WhiteLose << endl;
	cout << "AI1 WHITE fifty: " << ai1WhiteFiftyMove << endl;
	cout << "AI1 WHITE threefold: " << ai1WhiteThreefold << endl;
	cout << "AI1 WHITE I had no legals: " << ai1WhiteINoLegals << endl;
	cout << "AI1 WHITE they had no legals: " << ai1WhiteTheyNoLegals << endl;
	cout << "AI1 WHITE average move count: " << ai1WhiteMovePlayed / ((GAME_COUNT + 1) / 2) << endl;
	cout << "AI1 WHITE average time per move: " << ai1TotalWhiteTime / ai1WhiteMovePlayed << endl << endl;

	cout << "AI1 BLACK played: " << GAME_COUNT / 2 << endl;
	cout << "AI1 BLACK won: " << ai1BlackWin << endl;
	cout << "AI1 BLACK lost: " << ai1BlackLose << endl;
	cout << "AI1 BLACK fifty: " << ai1BlackFiftyMove << endl;
	cout << "AI1 BLACK threefold: " << ai1BlackThreefold << endl;
	cout << "AI1 BLACK I had no legals: " << ai1BlackINoLegals << endl;
	cout << "AI1 BLACK they had no legals: " << ai1BlackTheyNoLegals << endl;
	cout << "AI1 BLACK average move count: " << ai1BlackMovePlayed / (GAME_COUNT / 2) << endl;
	cout << "AI1 BLACK average time per move: " << ai1TotalBlackTime / ai1BlackMovePlayed << endl << endl;

	cout << "AI1 ALL played: " << GAME_COUNT << endl;
	cout << "AI1 ALL won: " << ai1WhiteWin + ai1BlackWin << endl;
	cout << "AI1 ALL lost: " << ai1WhiteLose + ai1BlackLose << endl;
	cout << "AI1 ALL fifty: " << ai1WhiteFiftyMove + ai1BlackFiftyMove << endl;
	cout << "AI1 ALL threefold: " << ai1WhiteThreefold + ai1BlackThreefold << endl;
	cout << "AI1 ALL I had no legals: " << ai1WhiteINoLegals + ai1BlackINoLegals << endl;
	cout << "AI1 ALL they had no legals: " << ai1WhiteTheyNoLegals + ai1BlackTheyNoLegals << endl;
	cout << "AI1 ALL average move count: " << (ai1WhiteMovePlayed + ai1BlackMovePlayed) / GAME_COUNT << endl;
	cout << "AI1 ALL average time per move: " << (ai1TotalWhiteTime + ai1TotalBlackTime) / (ai1WhiteMovePlayed + ai1BlackMovePlayed) << endl << endl;

	cout << "AI2 WHITE played: " << GAME_COUNT / 2 << endl;
	cout << "AI2 WHITE won: " << ai2WhiteWin << endl;
	cout << "AI2 WHITE lost: " << ai2WhiteLose << endl;
	cout << "AI2 WHITE fifty: " << ai2WhiteFiftyMove << endl;
	cout << "AI2 WHITE threefold: " << ai2WhiteThreefold << endl;
	cout << "AI2 WHITE I had no legals: " << ai2WhiteINoLegals << endl;
	cout << "AI2 WHITE they had no legals: " << ai2WhiteTheyNoLegals << endl;
	cout << "AI2 WHITE average move count: " << ai2WhiteMovePlayed / (GAME_COUNT / 2) << endl;
	cout << "AI2 WHITE average time per move: " << ai2TotalWhiteTime / ai2WhiteMovePlayed << endl << endl;

	cout << "AI2 BLACK played: " << (GAME_COUNT + 1) / 2 << endl;
	cout << "AI2 BLACK won: " << ai2BlackWin << endl;
	cout << "AI2 BLACK lost: " << ai2BlackLose << endl;
	cout << "AI2 BLACK fifty: " << ai2BlackFiftyMove << endl;
	cout << "AI2 BLACK threefold: " << ai2BlackThreefold << endl;
	cout << "AI2 BLACK I had no legals: " << ai2BlackINoLegals << endl;
	cout << "AI2 BLACK they had no legals: " << ai2BlackTheyNoLegals << endl;
	cout << "AI2 BLACK average move count: " << ai2BlackMovePlayed / ((GAME_COUNT + 1) / 2) << endl;
	cout << "AI2 BLACK average time per move: " << ai2TotalBlackTime / ai2BlackMovePlayed << endl << endl;

	cout << "AI2 ALL played: " << GAME_COUNT << endl;
	cout << "AI2 ALL won: " << ai2WhiteWin + ai2BlackWin << endl;
	cout << "AI2 ALL lost: " << ai2WhiteLose + ai2BlackLose << endl;
	cout << "AI2 ALL fifty: " << ai2WhiteFiftyMove + ai2BlackFiftyMove << endl;
	cout << "AI2 ALL threefold: " << ai2WhiteThreefold + ai2BlackThreefold << endl;
	cout << "AI2 ALL I had no legals: " << ai2WhiteINoLegals + ai2BlackINoLegals << endl;
	cout << "AI2 ALL they had no legals: " << ai2WhiteTheyNoLegals + ai2BlackTheyNoLegals << endl;
	cout << "AI2 ALL average move count: " << (ai2WhiteMovePlayed + ai2BlackMovePlayed) / GAME_COUNT << endl;
	cout << "AI2 ALL average time per move: " << (ai2TotalWhiteTime + ai2TotalBlackTime) / (ai2WhiteMovePlayed + ai2BlackMovePlayed) << endl << endl;

	bool moarGames;
	cout << "Moar games?" << endl;
	cin >> moarGames;

	if (moarGames)goto moarGamesLabel;

	return 0;
}

