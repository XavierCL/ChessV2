#ifndef GAMEUI_H
#define GAMEUI_H

#include "../Libraries/Headers/Artificial/Artificial.h"
#include "../Libraries/Headers/Physics/Point.h"

#include "../Libraries/Headers/utils/RandomEngineGenerator.h"

#include "resource.h"

#include <Windows.h>
#include <thread>
#include <random>
#include <fstream>

class GameUI
{
public:
	const static unsigned int WIDTH=450;
	const static unsigned int HEIGHT=WIDTH+100;
	const static unsigned int WIDTHm20=WIDTH-20;
	const static unsigned int HEIGHTm120=HEIGHT-120;
	const static float RECT_WIDTH;
	const static float RECT_HEIGHT;
	const static unsigned short INNER_RECT_WIDTH;
	const static unsigned short INNER_RECT_HEIGHT;
	const static unsigned short OUTER_PIECE_WIDTH;
	const static unsigned short OUTER_PIECE_HEIGHT;

	GameUI();
	void updateHandlers(const HWND &hwnd, const HINSTANCE &hinstance);
	void refreshDisplay();
	void toggleDisplay();
	void createUI(const HWND &hwnd, const HINSTANCE &hinstance);
	void singlePlayerClicked();
	void multiPlayerClicked();
	void noPlayerClicked();
	void startNewGame();
	void abortGame();
	void mouseDown(const short &x, const short &y);
	void mouseMove(const short &x, const short &y);
	void mouseUp(const short &x, const short &y);
	~GameUI();
private:

	void toggleMenu();
	void toggleGame(Move const * lastMoves=NULL, Point* toIgnore=NULL, bool updateScreen=true);
	void computerPreparation();
	void computerPlay();
	void playerTurn();
	void hideMenu();
	void hideGame();
	void drawPieceToMouse(HDC &hdc, const short &x, const short &y, Piece* piece);
	bool launchEndGame(const bool &showMessage=true);
	void selectRightPieceBitmap(HDC &hdc, const Piece* piece);

	HBRUSH white;
	HBRUSH darkBrown;
	HBRUSH green;
	HBRUSH blue;
	HBRUSH paleBrown;
	HBRUSH black;
	HBRUSH red;
	
	HDC _hdc=NULL;
	HDC _boardHdc=NULL;
	HDC _boardAndTokenHdc=NULL;
	HDC _boardMovingHdc=NULL;
	HDC _pieceHdc=NULL;
	HBITMAP _boardBitmap=NULL;
	HBITMAP _boardAndTokenBitmap=NULL;
	HBITMAP _boardMovingBitmap=NULL;
	bool _isHdcUpToDate;

	HWND _hwnd;
	HINSTANCE _hinstance;
	HBITMAP _backGroundBitmap;
	HWND _gameTypeGroupBox;
	HWND _singlePlayerRadioButton;
	HWND _multiPlayerRadioButton;
	HWND _noPlayerRadioButton;
	HWND _beginsGroupBox;
	HWND _randomBeginsRadioButton;
	HWND _computerBeginsRadioButton;
	HWND _playerBeginsRadioButton;
	HWND _difficultyLevelGroupBox;
	HWND _level1RadioButton;
	HWND _level2RadioButton;
	HWND _level3RadioButton;
	HWND _level4RadioButton;
	HWND _startButton;
	HWND _resumeButton;

	HWND _toMenuButton;
	HWND _restartButton;
	HWND _abortButton;
	HWND _knightUpgradeButton;
	HWND _bishopUpgradeButton;
	HWND _rookUpgradeButton;
	HWND _queenUpgradeButton;

	HBITMAP _whiteBishopBitmap;
	HBITMAP _whiteKingBitmap;
	HBITMAP _whiteKnightBitmap;
	HBITMAP _whitePawnBitmap;
	HBITMAP _whiteQueenBitmap;
	HBITMAP _whiteRookBitmap;
	HBITMAP _blackBishopBitmap;
	HBITMAP _blackKingBitmap;
	HBITMAP _blackKnightBitmap;
	HBITMAP _blackPawnBitmap;
	HBITMAP _blackQueenBitmap;
	HBITMAP _blackRookBitmap;

	bool _isMenuDisplayed=true;
	bool _isMouseDown=false;
	Move* _pointsMouseCovered;
	Point* _lastPoint;
	bool _isWhiteBottom;

	GameSet* _gameSet;
	Artificial* _artificial;
	Artificial* _artificial2;
	bool _isSinglePlayer;
	bool _isComputerWhite;
	bool _isComputerTurn;
	bool _isPlayerTurn;
	std::minstd_rand0* _randomGenerator;
	RandomEngineGenerator _randomEngineGenerator;
	bool _isComputerOnly;
	std::vector<Move const *>* _legals;
};

#endif