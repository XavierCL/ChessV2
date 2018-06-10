#include "GameUI.h"

#include "../Libraries/Headers/Moves/Factories/BitBoardMoveConstants.h"
#include "../Libraries/Headers/Artificial/BruteRecurseArtificial.h"
#include "../Libraries/Headers/Artificial/HeuristicSelective/HeuristicSelectiveArtificial.h"
#include "../Libraries/Headers/Artificial/HeuristicSelectiveNoDepth/HeuristicSelectiveArtificialND.h"
#include "../Libraries/Headers/Artificial/ProbabilityHeuristicSelective/ProbabilityHeuristicSelectiveArtificial.h"

#include <ctime>

using namespace std;

GameUI::GameUI()
	: _randomEngineGenerator(clock())
{
	_randomGenerator = new std::minstd_rand0(_randomEngineGenerator.next());
	_gameSet = NULL;
	_pointsMouseCovered = NULL;
	white = CreateSolidBrush(RGB(255, 255, 255));
	darkBrown = CreateSolidBrush(RGB(140, 82, 66));
	green = CreateSolidBrush(RGB(44, 162, 26));
	blue = CreateSolidBrush(RGB(30, 144, 255));
	paleBrown = CreateSolidBrush(RGB(255, 255, 206));
	black = CreateSolidBrush(RGB(0, 0, 0));
	red = CreateSolidBrush(RGB(247, 67, 67));
	_isHdcUpToDate = false;
	_legals = NULL;
	_isWhiteBottom = true;
	BitBoardMoveConstants::initialize();
	Logger::registerInfo([](const std::string& message) {
		OutputDebugStringA(message.c_str());
	});
}

void GameUI::updateHandlers(const HWND &hwnd, const HINSTANCE &hinstance)
{
	if (_hdc)ReleaseDC(_hwnd, _hdc);
	if (_boardMovingHdc)ReleaseDC(_hwnd, _boardMovingHdc);
	if (_boardAndTokenHdc)ReleaseDC(_hwnd, _boardAndTokenHdc);
	if (_boardHdc)ReleaseDC(_hwnd, _boardHdc);
	if (_pieceHdc)ReleaseDC(_hwnd, _pieceHdc);
	if (_boardAndTokenBitmap)DeleteObject(_boardAndTokenBitmap);
	if (_boardBitmap)DeleteObject(_boardBitmap);
	if (_boardMovingBitmap)DeleteObject(_boardMovingBitmap);

	_hwnd = hwnd;
	_hinstance = hinstance;

	_hdc = GetDC(hwnd);

	_boardMovingHdc = CreateCompatibleDC(_hdc);
	_boardAndTokenHdc = CreateCompatibleDC(_boardMovingHdc);
	_boardHdc = CreateCompatibleDC(_boardAndTokenHdc);
	_pieceHdc = CreateCompatibleDC(_boardAndTokenHdc);
	_boardMovingBitmap = CreateCompatibleBitmap(_hdc, WIDTH, HEIGHT - 82);
	_boardAndTokenBitmap = CreateCompatibleBitmap(_hdc, WIDTH, HEIGHT - 82);
	_boardBitmap = CreateCompatibleBitmap(_hdc, WIDTH, HEIGHT - 82);
	SelectObject(_boardMovingHdc, _boardMovingBitmap);
	SelectObject(_boardHdc, _boardBitmap);
	SelectObject(_boardAndTokenHdc, _boardAndTokenBitmap);

	SelectObject(_boardHdc, white);
	Rectangle(_boardHdc, 0, 0, WIDTH, HEIGHT - 82);

	SelectObject(_boardHdc, darkBrown);
	Rectangle(_boardHdc, 20, 0, WIDTH, HEIGHT - 120);

	SelectObject(_boardHdc, paleBrown);
	short counterX = 0;
	short counterY;
	while (counterX < Board::NUMBER_COLUMN)
	{
		counterY = (counterX % 2 == 1);
		while (counterY < Board::NUMBER_ROW)
		{
			Rectangle(_boardHdc, (int)floor(20 + counterX * RECT_WIDTH), (int)floor(counterY*RECT_HEIGHT), (int)ceil(20 + (counterX + 1)*RECT_WIDTH), (int)ceil((counterY + 1)*RECT_HEIGHT));
			counterY += 2;
		}
		++counterX;
	}

	_blackBishopBitmap = (HBITMAP)LoadBitmap(hinstance, MAKEINTRESOURCE(IDB_BLACK_BISHOP));
	_blackKingBitmap = (HBITMAP)LoadBitmap(hinstance, MAKEINTRESOURCE(IDB_BLACK_KING));
	_blackKnightBitmap = (HBITMAP)LoadBitmap(hinstance, MAKEINTRESOURCE(IDB_BLACK_KNIGHT));
	_blackPawnBitmap = (HBITMAP)LoadBitmap(hinstance, MAKEINTRESOURCE(IDB_BLACK_PAWN));
	_blackQueenBitmap = (HBITMAP)LoadBitmap(hinstance, MAKEINTRESOURCE(IDB_BLACK_QUEEN));
	_blackRookBitmap = (HBITMAP)LoadBitmap(hinstance, MAKEINTRESOURCE(IDB_BLACK_ROOK));

	_whiteBishopBitmap = (HBITMAP)LoadBitmap(hinstance, MAKEINTRESOURCE(IDB_WHITE_BISHOP));
	_whiteKingBitmap = (HBITMAP)LoadBitmap(hinstance, MAKEINTRESOURCE(IDB_WHITE_KING));
	_whiteKnightBitmap = (HBITMAP)LoadBitmap(hinstance, MAKEINTRESOURCE(IDB_WHITE_KNIGHT));
	_whitePawnBitmap = (HBITMAP)LoadBitmap(hinstance, MAKEINTRESOURCE(IDB_WHITE_PAWN));
	_whiteQueenBitmap = (HBITMAP)LoadBitmap(hinstance, MAKEINTRESOURCE(IDB_WHITE_QUEEN));
	_whiteRookBitmap = (HBITMAP)LoadBitmap(hinstance, MAKEINTRESOURCE(IDB_WHITE_ROOK));
}

void GameUI::createUI(const HWND &hwnd, const HINSTANCE &hinstance)
{
	_backGroundBitmap = (HBITMAP)LoadBitmap(hinstance, MAKEINTRESOURCE(IDB_BLUE_FONT));
	HWND backGround = CreateWindow(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | SS_BITMAP, 0, 0, 0, 0, hwnd, 0, hinstance, NULL);

	//menu Page UI
	_gameTypeGroupBox = CreateWindow(L"BUTTON", L"Select game type:", WS_VISIBLE | WS_CHILD | BS_GROUPBOX, 20, 2 * HEIGHT / 5, (GameUI::WIDTH - 4 * 20) / 3, 2 * GameUI::HEIGHT / 5, hwnd, (HMENU)0, hinstance, NULL);
	_singlePlayerRadioButton = CreateWindow(L"BUTTON", L"Single Player", WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON | WS_GROUP, 25, 2 * HEIGHT / 5 + GameUI::HEIGHT / 10, (GameUI::WIDTH - 4 * 20) / 3 - 10, 20, hwnd, (HMENU)1, hinstance, NULL);
	_multiPlayerRadioButton = CreateWindow(L"BUTTON", L"Multiplayer", WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON, 25, 2 * HEIGHT / 5 + GameUI::HEIGHT / 5, (GameUI::WIDTH - 4 * 20) / 3 - 10, 20, hwnd, (HMENU)2, hinstance, NULL);
	_noPlayerRadioButton = CreateWindow(L"BUTTON", L"Computer Only", WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON, 25, 2 * HEIGHT / 5 + 3 * GameUI::HEIGHT / 10, (GameUI::WIDTH - 4 * 20) / 3 - 10, 20, hwnd, (HMENU)14, hinstance, NULL);

	SendMessage(_singlePlayerRadioButton, BM_SETCHECK, BST_CHECKED, 0);

	_beginsGroupBox = CreateWindow(L"BUTTON", L"Select who starts:", WS_VISIBLE | WS_CHILD | BS_GROUPBOX, 40 + (GameUI::WIDTH - 4 * 20) / 3, 2 * HEIGHT / 5, (GameUI::WIDTH - 4 * 20) / 3, 2 * GameUI::HEIGHT / 5, hwnd, (HMENU)3, hinstance, NULL);
	_randomBeginsRadioButton = CreateWindow(L"BUTTON", L"Random starts", WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON | WS_GROUP, 45 + (GameUI::WIDTH - 4 * 20) / 3, 2 * HEIGHT / 5 + GameUI::HEIGHT / 10, (GameUI::WIDTH - 4 * 20) / 3 - 10, 20, hwnd, (HMENU)4, hinstance, NULL);
	_computerBeginsRadioButton = CreateWindow(L"BUTTON", L"Computer starts", WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON, 45 + (GameUI::WIDTH - 4 * 20) / 3, 2 * HEIGHT / 5 + GameUI::HEIGHT / 5, (GameUI::WIDTH - 4 * 20) / 3 - 10, 20, hwnd, (HMENU)5, hinstance, NULL);
	_playerBeginsRadioButton = CreateWindow(L"BUTTON", L"Player starts", WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON, 45 + (GameUI::WIDTH - 4 * 20) / 3, 2 * HEIGHT / 5 + 3 * GameUI::HEIGHT / 10, (GameUI::WIDTH - 4 * 20) / 3 - 10, 20, hwnd, (HMENU)6, hinstance, NULL);

	SendMessage(_randomBeginsRadioButton, BM_SETCHECK, BST_CHECKED, 0);

	_difficultyLevelGroupBox = CreateWindow(L"BUTTON", L"Select computer level:", WS_VISIBLE | WS_CHILD | BS_GROUPBOX, 60 + 2 * (GameUI::WIDTH - 4 * 20) / 3, 2 * HEIGHT / 5, (GameUI::WIDTH - 4 * 20) / 3, 2 * GameUI::HEIGHT / 5, hwnd, (HMENU)7, hinstance, NULL);
	_level1RadioButton = CreateWindow(L"BUTTON", L"Difficulty 1", WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON | WS_GROUP, 65 + 2 * (GameUI::WIDTH - 4 * 20) / 3, 2 * HEIGHT / 5 + 2 * GameUI::HEIGHT / 25, (GameUI::WIDTH - 4 * 20) / 3 - 10, 20, hwnd, (HMENU)8, hinstance, NULL);
	_level2RadioButton = CreateWindow(L"BUTTON", L"Difficulty 2", WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON, 65 + 2 * (GameUI::WIDTH - 4 * 20) / 3, 2 * HEIGHT / 5 + 4 * GameUI::HEIGHT / 25, (GameUI::WIDTH - 4 * 20) / 3 - 10, 20, hwnd, (HMENU)9, hinstance, NULL);
	_level3RadioButton = CreateWindow(L"BUTTON", L"Difficulty 3", WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON, 65 + 2 * (GameUI::WIDTH - 4 * 20) / 3, 2 * HEIGHT / 5 + 6 * GameUI::HEIGHT / 25, (GameUI::WIDTH - 4 * 20) / 3 - 10, 20, hwnd, (HMENU)10, hinstance, NULL);
	_level4RadioButton = CreateWindow(L"BUTTON", L"Difficulty 4", WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON, 65 + 2 * (GameUI::WIDTH - 4 * 20) / 3, 2 * HEIGHT / 5 + 8 * GameUI::HEIGHT / 25, (GameUI::WIDTH - 4 * 20) / 3 - 10, 20, hwnd, (HMENU)13, hinstance, NULL);

	SendMessage(_level2RadioButton, BM_SETCHECK, BST_CHECKED, 0);

	_startButton = CreateWindow(L"BUTTON", L"Start!", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, WIDTH / 2 - 50, HEIGHT - 80, 100, 25, hwnd, (HMENU)11, hinstance, NULL);
	_resumeButton = CreateWindow(L"BUTTON", L"resume -->", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, WIDTH - 87, HEIGHT - 80, 70, 25, hwnd, (HMENU)12, hinstance, NULL);

	EnableWindow(_resumeButton, false);

	//game page UI
	_toMenuButton = CreateWindow(L"BUTTON", L"Restart!", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, WIDTH / 2 - 50, HEIGHT - 80, 100, 25, hwnd, (HMENU)11, hinstance, NULL);
	_restartButton = CreateWindow(L"BUTTON", L"<-- menu", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 17, HEIGHT - 80, 70, 25, hwnd, (HMENU)12, hinstance, NULL);
	_abortButton = CreateWindow(L"BUTTON", L"Abort Game", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 3 * WIDTH / 4 - 50, HEIGHT - 80, 100, 25, hwnd, (HMENU)15, hinstance, NULL);

	ShowWindow(_toMenuButton, SW_HIDE);
	ShowWindow(_restartButton, SW_HIDE);
	ShowWindow(_abortButton, SW_HIDE);

	EnableWindow(_abortButton, false);
}

void GameUI::refreshDisplay()
{
	if (_isMenuDisplayed)
	{
		toggleMenu();
	}
	else
	{
		toggleGame();
	}
}

void GameUI::toggleDisplay()
{
	if (_isMenuDisplayed)
	{
		hideMenu();
		toggleGame();
		toggleGame();
	}
	else
	{
		EnableWindow(_resumeButton, true);
		hideGame();
		toggleMenu();
	}
	_isMenuDisplayed = !_isMenuDisplayed;
}

void GameUI::toggleMenu()
{
	SendDlgItemMessage(_hwnd, 0, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)_backGroundBitmap);

	SelectObject(_hdc, white);
	Rectangle(_hdc, 17, 297, 23 + (WIDTH - 4 * 20) / 3, 303 + 2 * HEIGHT / 5);
	Rectangle(_hdc, 37 + (WIDTH - 4 * 20) / 3, 297, 43 + 2 * (WIDTH - 4 * 20) / 3, 303 + 2 * HEIGHT / 5);
	Rectangle(_hdc, 57 + 2 * (WIDTH - 4 * 20) / 3, 297, WIDTH - 17, 303 + 2 * HEIGHT / 5);

	ShowWindow(_gameTypeGroupBox, SW_SHOW);
	ShowWindow(_singlePlayerRadioButton, SW_SHOW);
	ShowWindow(_multiPlayerRadioButton, SW_SHOW);
	ShowWindow(_noPlayerRadioButton, SW_SHOW);
	ShowWindow(_beginsGroupBox, SW_SHOW);
	ShowWindow(_randomBeginsRadioButton, SW_SHOW);
	ShowWindow(_computerBeginsRadioButton, SW_SHOW);
	ShowWindow(_playerBeginsRadioButton, SW_SHOW);
	ShowWindow(_difficultyLevelGroupBox, SW_SHOW);
	ShowWindow(_level1RadioButton, SW_SHOW);
	ShowWindow(_level2RadioButton, SW_SHOW);
	ShowWindow(_level3RadioButton, SW_SHOW);
	ShowWindow(_level4RadioButton, SW_SHOW);
	ShowWindow(_startButton, SW_SHOW);
	ShowWindow(_resumeButton, SW_SHOW);

	SendMessage(_gameTypeGroupBox, WM_SETFONT, (WPARAM)GetStockObject(ANSI_VAR_FONT), (LPARAM)MAKELONG(TRUE, 0));
	SendMessage(_singlePlayerRadioButton, WM_SETFONT, (WPARAM)GetStockObject(ANSI_VAR_FONT), (LPARAM)MAKELONG(TRUE, 0));
	SendMessage(_multiPlayerRadioButton, WM_SETFONT, (WPARAM)GetStockObject(ANSI_VAR_FONT), (LPARAM)MAKELONG(TRUE, 0));
	SendMessage(_noPlayerRadioButton, WM_SETFONT, (WPARAM)GetStockObject(ANSI_VAR_FONT), (LPARAM)MAKELONG(TRUE, 0));

	SendMessage(_beginsGroupBox, WM_SETFONT, (WPARAM)GetStockObject(ANSI_VAR_FONT), (LPARAM)MAKELONG(TRUE, 0));
	SendMessage(_randomBeginsRadioButton, WM_SETFONT, (WPARAM)GetStockObject(ANSI_VAR_FONT), (LPARAM)MAKELONG(TRUE, 0));
	SendMessage(_computerBeginsRadioButton, WM_SETFONT, (WPARAM)GetStockObject(ANSI_VAR_FONT), (LPARAM)MAKELONG(TRUE, 0));
	SendMessage(_playerBeginsRadioButton, WM_SETFONT, (WPARAM)GetStockObject(ANSI_VAR_FONT), (LPARAM)MAKELONG(TRUE, 0));

	SendMessage(_difficultyLevelGroupBox, WM_SETFONT, (WPARAM)GetStockObject(ANSI_VAR_FONT), (LPARAM)MAKELONG(TRUE, 0));
	SendMessage(_level1RadioButton, WM_SETFONT, (WPARAM)GetStockObject(ANSI_VAR_FONT), (LPARAM)MAKELONG(TRUE, 0));
	SendMessage(_level2RadioButton, WM_SETFONT, (WPARAM)GetStockObject(ANSI_VAR_FONT), (LPARAM)MAKELONG(TRUE, 0));
	SendMessage(_level3RadioButton, WM_SETFONT, (WPARAM)GetStockObject(ANSI_VAR_FONT), (LPARAM)MAKELONG(TRUE, 0));
	SendMessage(_level4RadioButton, WM_SETFONT, (WPARAM)GetStockObject(ANSI_VAR_FONT), (LPARAM)MAKELONG(TRUE, 0));

	SendMessage(_startButton, WM_SETFONT, (WPARAM)GetStockObject(ANSI_VAR_FONT), (LPARAM)MAKELONG(TRUE, 0));
	SendMessage(_resumeButton, WM_SETFONT, (WPARAM)GetStockObject(ANSI_VAR_FONT), (LPARAM)MAKELONG(TRUE, 0));
}

void GameUI::toggleGame(Move const * lastMoves, Point* toIgnore, bool updateScreen)
{
	if (_isMenuDisplayed)
	{
		SendDlgItemMessage(_hwnd, 0, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)_backGroundBitmap);
	}

	if (!_isHdcUpToDate)
	{
		BitBlt(_boardAndTokenHdc, 0, 0, WIDTH, HEIGHT - 82, _boardHdc, 0, 0, SRCCOPY);

		if (lastMoves != NULL)
		{
			if (_isWhiteBottom)
			{
				SelectObject(_boardAndTokenHdc, blue);
				Rectangle(_boardAndTokenHdc,
					20 + (Board::NUMBER_COLUMN - Point(lastMoves->movingPieceDestination()).x() - 1)*RECT_WIDTH,
					(Board::NUMBER_ROW - Point(lastMoves->movingPieceDestination()).y() - 1)*RECT_HEIGHT,
					20 + (Board::NUMBER_COLUMN - Point(lastMoves->movingPieceDestination()).x())*RECT_WIDTH,
					(Board::NUMBER_ROW - Point(lastMoves->movingPieceDestination()).y())*RECT_HEIGHT);
				SelectObject(_boardAndTokenHdc, Point(lastMoves->movingPieceDestination()).x() % 2 == Point(lastMoves->movingPieceDestination()).y() % 2 ? paleBrown : darkBrown);
				Rectangle(_boardAndTokenHdc,
					20 + (Board::NUMBER_COLUMN - Point(lastMoves->movingPieceDestination()).x() - 1)*RECT_WIDTH + RECT_WIDTH / 2 - INNER_RECT_WIDTH / 2,
					(Board::NUMBER_ROW - Point(lastMoves->movingPieceDestination()).y() - 1)*RECT_HEIGHT + RECT_HEIGHT / 2 - INNER_RECT_HEIGHT / 2,
					20 + (Board::NUMBER_COLUMN - Point(lastMoves->movingPieceDestination()).x())*RECT_WIDTH - RECT_WIDTH / 2 + INNER_RECT_WIDTH / 2,
					(Board::NUMBER_ROW - Point(lastMoves->movingPieceDestination()).y())*RECT_HEIGHT - RECT_HEIGHT / 2 + INNER_RECT_HEIGHT / 2);
				SelectObject(_boardAndTokenHdc, green);
				Rectangle(_boardAndTokenHdc,
					20 + (Board::NUMBER_COLUMN - Point(lastMoves->movingPieceOrigin()).x() - 1)*RECT_WIDTH,
					(Board::NUMBER_ROW - Point(lastMoves->movingPieceOrigin()).y() - 1)*RECT_HEIGHT,
					20 + (Board::NUMBER_COLUMN - Point(lastMoves->movingPieceOrigin()).x())*RECT_WIDTH,
					(Board::NUMBER_ROW - Point(lastMoves->movingPieceOrigin()).y())*RECT_HEIGHT);
				SelectObject(_boardAndTokenHdc, Point(lastMoves->movingPieceOrigin()).x() % 2 == Point(lastMoves->movingPieceOrigin()).y() % 2 ? paleBrown : darkBrown);
				Rectangle(_boardAndTokenHdc,
					20 + (Board::NUMBER_COLUMN - Point(lastMoves->movingPieceOrigin()).x() - 1)*RECT_WIDTH + RECT_WIDTH / 2 - INNER_RECT_WIDTH / 2,
					(Board::NUMBER_ROW - Point(lastMoves->movingPieceOrigin()).y() - 1)*RECT_HEIGHT + RECT_HEIGHT / 2 - INNER_RECT_HEIGHT / 2,
					20 + (Board::NUMBER_COLUMN - Point(lastMoves->movingPieceOrigin()).x())*RECT_WIDTH - RECT_WIDTH / 2 + INNER_RECT_WIDTH / 2,
					(Board::NUMBER_ROW - Point(lastMoves->movingPieceOrigin()).y())*RECT_HEIGHT - RECT_HEIGHT / 2 + INNER_RECT_HEIGHT / 2);
			}
			else
			{
				SelectObject(_boardAndTokenHdc, blue);
				Rectangle(_boardAndTokenHdc,
					20 + (Board::NUMBER_COLUMN - Point(lastMoves->movingPieceDestination()).x() - 1)*RECT_WIDTH,
					Point(lastMoves->movingPieceDestination()).y()*RECT_HEIGHT,
					20 + (Board::NUMBER_COLUMN - Point(lastMoves->movingPieceDestination()).x())*RECT_WIDTH,
					(Point(lastMoves->movingPieceDestination()).y() + 1)*RECT_HEIGHT);
				SelectObject(_boardAndTokenHdc, Point(lastMoves->movingPieceDestination()).x() % 2 != Point(lastMoves->movingPieceDestination()).y() % 2 ? paleBrown : darkBrown);
				Rectangle(_boardAndTokenHdc,
					20 + (Board::NUMBER_COLUMN - Point(lastMoves->movingPieceDestination()).x() - 1)*RECT_WIDTH + RECT_WIDTH / 2 - INNER_RECT_WIDTH / 2,
					Point(lastMoves->movingPieceDestination()).y()*RECT_HEIGHT + RECT_HEIGHT / 2 - INNER_RECT_HEIGHT / 2,
					20 + (Board::NUMBER_COLUMN - Point(lastMoves->movingPieceDestination()).x())*RECT_WIDTH - RECT_WIDTH / 2 + INNER_RECT_WIDTH / 2,
					(Point(lastMoves->movingPieceDestination()).y() + 1)*RECT_HEIGHT - RECT_HEIGHT / 2 + INNER_RECT_HEIGHT / 2);
				SelectObject(_boardAndTokenHdc, green);
				Rectangle(_boardAndTokenHdc,
					20 + (Board::NUMBER_COLUMN - Point(lastMoves->movingPieceOrigin()).x() - 1)*RECT_WIDTH,
					Point(lastMoves->movingPieceOrigin()).y()*RECT_HEIGHT,
					20 + (Board::NUMBER_COLUMN - Point(lastMoves->movingPieceOrigin()).x())*RECT_WIDTH,
					(Point(lastMoves->movingPieceOrigin()).y() + 1)*RECT_HEIGHT);
				SelectObject(_boardAndTokenHdc, Point(lastMoves->movingPieceOrigin()).x() % 2 != Point(lastMoves->movingPieceOrigin()).y() % 2 ? paleBrown : darkBrown);
				Rectangle(_boardAndTokenHdc,
					20 + (Board::NUMBER_COLUMN - Point(lastMoves->movingPieceOrigin()).x() - 1)*RECT_WIDTH + RECT_WIDTH / 2 - INNER_RECT_WIDTH / 2,
					Point(lastMoves->movingPieceOrigin()).y()*RECT_HEIGHT + RECT_HEIGHT / 2 - INNER_RECT_HEIGHT / 2,
					20 + (Board::NUMBER_COLUMN - Point(lastMoves->movingPieceOrigin()).x())*RECT_WIDTH - RECT_WIDTH / 2 + INNER_RECT_WIDTH / 2,
					(Point(lastMoves->movingPieceOrigin()).y() + 1)*RECT_HEIGHT - RECT_HEIGHT / 2 + INNER_RECT_HEIGHT / 2);
			}
		}

		Point ctoIgnore;
		if (toIgnore == NULL)ctoIgnore = Point(Board::NUMBER_COLUMN, Board::NUMBER_ROW);
		else ctoIgnore = *toIgnore;

		short counterX = 0;
		short counterY;
		while (counterX < Board::NUMBER_COLUMN)
		{
			counterY = 0;
			while (counterY < Board::NUMBER_ROW)
			{
				if (counterX == ctoIgnore.x() && ((_isWhiteBottom && counterY == ctoIgnore.y()) || (!_isWhiteBottom && counterY == Board::NUMBER_ROW - ctoIgnore.y() - 1)))
				{
					++counterY;
					continue;
				}
				Piece* currentPiece = new Piece(_gameSet->get(counterX, _isWhiteBottom ? counterY : Board::NUMBER_ROW - counterY - 1));
				if (!currentPiece->isEmpty())
				{
					selectRightPieceBitmap(_pieceHdc, currentPiece);

					BitBlt(_boardAndTokenHdc,
						(int)(20 + (Board::NUMBER_COLUMN - counterX - 1) * RECT_WIDTH - OUTER_PIECE_WIDTH / 2 + RECT_WIDTH / 2),
						(int)((Board::NUMBER_ROW - counterY - 1)*RECT_HEIGHT - OUTER_PIECE_WIDTH / 2 + RECT_HEIGHT / 2),
						(int)(INNER_RECT_WIDTH),
						(int)(INNER_RECT_HEIGHT),
						_pieceHdc, 0, 0, SRCCOPY);
				}
				++counterY;
			}
			++counterX;
		}
		_isHdcUpToDate = true;
	}

	BitBlt(_boardMovingHdc, 0, 0, WIDTH, HEIGHT - 82, _boardAndTokenHdc, 0, 0, SRCCOPY);
	if (updateScreen)BitBlt(_hdc, 0, 0, WIDTH, HEIGHT - 82, _boardMovingHdc, 0, 0, SRCCOPY);

	ShowWindow(_toMenuButton, SW_SHOW);
	ShowWindow(_restartButton, SW_SHOW);
	ShowWindow(_abortButton, SW_SHOW);

	SendMessage(_toMenuButton, WM_SETFONT, (WPARAM)GetStockObject(ANSI_VAR_FONT), (LPARAM)MAKELONG(TRUE, 0));
	SendMessage(_restartButton, WM_SETFONT, (WPARAM)GetStockObject(ANSI_VAR_FONT), (LPARAM)MAKELONG(TRUE, 0));
	SendMessage(_abortButton, WM_SETFONT, (WPARAM)GetStockObject(ANSI_VAR_FONT), (LPARAM)MAKELONG(TRUE, 0));
}

void GameUI::singlePlayerClicked()
{
	EnableWindow(_randomBeginsRadioButton, true);
	EnableWindow(_computerBeginsRadioButton, true);
	EnableWindow(_playerBeginsRadioButton, true);
	EnableWindow(_level1RadioButton, true);
	EnableWindow(_level2RadioButton, true);
	EnableWindow(_level3RadioButton, true);
	EnableWindow(_level4RadioButton, true);
}

void GameUI::multiPlayerClicked()
{
	EnableWindow(_randomBeginsRadioButton, false);
	EnableWindow(_computerBeginsRadioButton, false);
	EnableWindow(_playerBeginsRadioButton, false);
	EnableWindow(_level1RadioButton, false);
	EnableWindow(_level2RadioButton, false);
	EnableWindow(_level3RadioButton, false);
	EnableWindow(_level4RadioButton, false);
}

void GameUI::noPlayerClicked()
{
	EnableWindow(_randomBeginsRadioButton, false);
	EnableWindow(_computerBeginsRadioButton, false);
	EnableWindow(_playerBeginsRadioButton, false);
	EnableWindow(_level1RadioButton, true);
	EnableWindow(_level2RadioButton, true);
	EnableWindow(_level3RadioButton, true);
	EnableWindow(_level4RadioButton, true);
}

void GameUI::hideMenu()
{
	ShowWindow(_gameTypeGroupBox, SW_HIDE);
	ShowWindow(_singlePlayerRadioButton, SW_HIDE);
	ShowWindow(_multiPlayerRadioButton, SW_HIDE);
	ShowWindow(_noPlayerRadioButton, SW_HIDE);
	ShowWindow(_beginsGroupBox, SW_HIDE);
	ShowWindow(_randomBeginsRadioButton, SW_HIDE);
	ShowWindow(_computerBeginsRadioButton, SW_HIDE);
	ShowWindow(_playerBeginsRadioButton, SW_HIDE);
	ShowWindow(_difficultyLevelGroupBox, SW_HIDE);
	ShowWindow(_level1RadioButton, SW_HIDE);
	ShowWindow(_level2RadioButton, SW_HIDE);
	ShowWindow(_level3RadioButton, SW_HIDE);
	ShowWindow(_level4RadioButton, SW_HIDE);
	ShowWindow(_startButton, SW_HIDE);
	ShowWindow(_resumeButton, SW_HIDE);
}

void GameUI::hideGame()
{
	ShowWindow(_toMenuButton, SW_HIDE);
	ShowWindow(_restartButton, SW_HIDE);
	ShowWindow(_abortButton, SW_HIDE);
}

void GameUI::startNewGame()
{
	if (_gameSet)delete _gameSet;
	_gameSet = new GameSet();
	if (IsDlgButtonChecked(_hwnd, 1) == BST_CHECKED || IsDlgButtonChecked(_hwnd, 14) == BST_CHECKED)
	{
		_isSinglePlayer = true;
		if (IsDlgButtonChecked(_hwnd, 14) == BST_CHECKED)
		{
			_isComputerOnly = true;
			_isComputerWhite = true;
		}
		else
		{
			_isComputerOnly = false;
			if (IsDlgButtonChecked(_hwnd, 4) == BST_CHECKED)
			{
				uniform_int_distribution<int> aDistributor(0, 1);
				_isComputerWhite = aDistributor(*_randomGenerator);
			}
			else if (IsDlgButtonChecked(_hwnd, 5) == BST_CHECKED)
			{
				_isComputerWhite = true;
			}
			else
			{
				_isComputerWhite = false;
			}
			_isWhiteBottom = !_isComputerWhite;
		}
		if (_artificial)delete _artificial;
		if (_artificial2) delete _artificial2;
		if (IsDlgButtonChecked(_hwnd, 8) == BST_CHECKED)
		{
			_artificial = new BruteRecurseArtificial(0, _randomGenerator);
		}
		else if (IsDlgButtonChecked(_hwnd, 9) == BST_CHECKED)
		{
			_artificial = new BruteRecurseArtificial(3, _randomGenerator);
		}
		else if (IsDlgButtonChecked(_hwnd, 10) == BST_CHECKED)
		{
			_artificial = new HeuristicSelectiveArtificial(1000, 1000000, _randomEngineGenerator, FixedUnorderedMap<Board, std::shared_ptr<std::vector<Move const *>>, BoardHash>(1000000));
		}
		else
		{
			_artificial = new ProbabilityHeuristicSelectiveArtificial(7000, 1, 1000000000, _randomGenerator);
		}
		_artificial2 = new HeuristicSelectiveArtificial(7000, 20000, _randomEngineGenerator, FixedUnorderedMap<Board, std::shared_ptr<std::vector<Move const *>>, BoardHash>(1000000));
	}
	else
	{
		_isSinglePlayer = false;
		_isWhiteBottom = true;
	}
	hideMenu();
	_isHdcUpToDate = false;
	toggleGame();
	toggleGame();
	_isMenuDisplayed = false;
	if (_isSinglePlayer && _isComputerWhite)
	{
		computerPreparation();
	}
	else
	{
		playerTurn();
	}
}

void GameUI::computerPreparation()
{
	_isPlayerTurn = false;
	_isComputerTurn = true;
	EnableWindow(_toMenuButton, false);
	EnableWindow(_restartButton, false);
	EnableWindow(_abortButton, _isComputerOnly);

	thread{ &GameUI::computerPlay, this }.detach();
}

void GameUI::computerPlay()
{
	do
	{
		if (_gameSet->isWhiteTurn())
		{
			Move const * move = _artificial->getMove(*_gameSet);
			GameSet* temp = new GameSet(_gameSet->playMove(*move));
			delete _gameSet;
			_gameSet = temp;
			_isHdcUpToDate = false;
			toggleGame(move);
		}
		else
		{
			Move const * move = _artificial2->getMove(*_gameSet);
			GameSet* temp = new GameSet(_gameSet->playMove(*move));
			delete _gameSet;
			_gameSet = temp;
			_isHdcUpToDate = false;
			toggleGame(move);
		}
	} while (_isComputerTurn && _isComputerOnly && !launchEndGame(false));
	EnableWindow(_abortButton, false);
	EnableWindow(_toMenuButton, true);
	EnableWindow(_restartButton, true);
	_isComputerTurn = false;
	if (!launchEndGame())
	{
		if (!_isComputerOnly)playerTurn();
	}
}

void GameUI::abortGame()
{
	_isComputerTurn = false;
	EnableWindow(_abortButton, false);
}

void GameUI::playerTurn()
{
	_isComputerTurn = false;
	if (_pointsMouseCovered)
	{
		delete _pointsMouseCovered;
		_pointsMouseCovered = NULL;
	}
	_pointsMouseCovered = new SimpleMove(0, 0, PieceType::EMPTY);
	_legals = new vector<Move const *>(*_gameSet->getLegals());
	_isMouseDown = false;
	_isPlayerTurn = true;
}

void GameUI::mouseDown(const short &x, const short &y)
{
	if (!_isMenuDisplayed && _isPlayerTurn && x > 20 && y < HEIGHT - 120)
	{
		if (_isMouseDown)
		{
			mouseUp(x, y);
			return;
		}
		if (_pointsMouseCovered)
		{
			delete _pointsMouseCovered;
			_pointsMouseCovered = NULL;
		}
		short ySquare = Board::NUMBER_ROW*y / (HEIGHT - 120);
		short xSquare = Board::NUMBER_COLUMN - Board::NUMBER_COLUMN*(x - 20) / (WIDTH - 20) - 1;
		if (_isWhiteBottom)ySquare = Board::NUMBER_ROW - ySquare - 1;
		if ((_gameSet->isWhiteTurn() && _gameSet->get(xSquare, ySquare).isWhite()) || (!_gameSet->isWhiteTurn() && !_gameSet->get(xSquare, ySquare).isWhite()))
		{
			Point startingPoint(xSquare, ySquare);
			if (_lastPoint)delete _lastPoint;
			_lastPoint = new Point(startingPoint);
			unsigned short counter1 = 0;
			while (counter1 < _legals->size())
			{
				if (Point((*_legals)[counter1]->movingPieceOrigin()) == startingPoint)break;
				++counter1;
			}
			if (counter1 != _legals->size())
			{
				_isHdcUpToDate = false;
				_isMouseDown = true;
				_pointsMouseCovered = new SimpleMove(startingPoint.singleBitBoard(), 0, PieceType::EMPTY);
				toggleGame(_pointsMouseCovered);
				_isHdcUpToDate = false;
				return;
			}
		}
		toggleGame();
	}
	_isMouseDown = false;
}

void GameUI::mouseMove(const short &x, const short &y)
{
	if (!_isMenuDisplayed && _isPlayerTurn && _isMouseDown && x > 20 && y < HEIGHT - 120)
	{
		short ySquare = Board::NUMBER_ROW*y / HEIGHTm120;
		short xSquare = Board::NUMBER_COLUMN - Board::NUMBER_COLUMN*(x - 20) / WIDTHm20 - 1;
		if (_isWhiteBottom)ySquare = Board::NUMBER_ROW - ySquare - 1;
		Point currentPoint(xSquare, ySquare);
		if (currentPoint != *_lastPoint && currentPoint != _pointsMouseCovered->movingPieceDestination())
		{
			delete _lastPoint;
			_lastPoint = new Point(xSquare, ySquare);
			if (Point(_pointsMouseCovered->movingPieceOrigin()) == currentPoint)
			{
				_isHdcUpToDate = false;
				_pointsMouseCovered = new SimpleMove(currentPoint.singleBitBoard(), 0, PieceType::EMPTY);
			}
			else
			{
				Move* possiblePath = new SimpleMove(_pointsMouseCovered->movingPieceOrigin(), currentPoint.singleBitBoard(), PieceType::EMPTY);
				unsigned short counter1 = 0;
				while (counter1 < _legals->size())
				{
					if ((*_legals)[counter1]->movingPieceOrigin() == possiblePath->movingPieceOrigin()
						&& (*_legals)[counter1]->movingPieceDestination() == possiblePath->movingPieceDestination())
					{
						_isHdcUpToDate = false;
						if (_pointsMouseCovered)delete _pointsMouseCovered;
						_pointsMouseCovered = possiblePath;
						break;
					}
					++counter1;
				}
				if (counter1 == _legals->size())delete possiblePath;
			}
		}
		Point* temp = new Point(_pointsMouseCovered->movingPieceOrigin());
		toggleGame(_pointsMouseCovered, temp, false);
		delete temp;
		Piece* temp2 = new Piece(_gameSet->get(Point(_pointsMouseCovered->movingPieceOrigin()).x(), Point(_pointsMouseCovered->movingPieceOrigin()).y()));
		drawPieceToMouse(_boardMovingHdc, x, y, temp2);
		delete temp2;
		BitBlt(_hdc, 0, 0, WIDTH, HEIGHT - 82, _boardMovingHdc, 0, 0, SRCCOPY);
		return;
	}
}

void GameUI::mouseUp(const short &x, const short &y)
{
	bool movePlayed = false;
	if (!_isMenuDisplayed && _isPlayerTurn)
	{
		if (_isMouseDown && x > 20 && y < HEIGHT - 120)
		{
			short ySquare = Board::NUMBER_ROW*y / (HEIGHT - 120);
			short xSquare = Board::NUMBER_COLUMN - Board::NUMBER_COLUMN*(x - 20) / (WIDTH - 20) - 1;
			if (_isWhiteBottom)ySquare = Board::NUMBER_ROW - ySquare - 1;
			if (_lastPoint)delete _lastPoint;
			_lastPoint = NULL;
			Point currentPoint(xSquare, ySquare);
			if (currentPoint == _pointsMouseCovered->movingPieceDestination())
			{
				unsigned int counter1 = 0;
				while (counter1 < _legals->size())
				{
					if ((*_legals)[counter1]->movingPieceOrigin() == _pointsMouseCovered->movingPieceOrigin()
						&& (*_legals)[counter1]->movingPieceDestination() == _pointsMouseCovered->movingPieceDestination())
					{
						GameSet* temp = new GameSet(_gameSet->playMove(*(*_legals)[counter1]));
						delete _gameSet;
						_gameSet = temp;
						movePlayed = true;
						break;
					}
					++counter1;
				}
			}
		}
		if (movePlayed)_isPlayerTurn = false;
		_isHdcUpToDate = false;
		toggleGame(_pointsMouseCovered);
		delete _pointsMouseCovered;
		_pointsMouseCovered = NULL;
		if (movePlayed && !launchEndGame())
		{
			if (_isSinglePlayer)computerPreparation();
			else playerTurn();
		}
	}
	_isMouseDown = false;
}

void GameUI::drawPieceToMouse(HDC &hdc, const short &x, const short &y, Piece* piece)
{
	selectRightPieceBitmap(_pieceHdc, piece);

	//GdiTransparentBlt(hdc, x-OUTER_PIECE_WIDTH/2, y-OUTER_PIECE_HEIGHT/2, OUTER_PIECE_WIDTH, OUTER_PIECE_HEIGHT, _pieceHdc, 0, 0, OUTER_PIECE_WIDTH, OUTER_PIECE_HEIGHT, RGB(255,255,255));
	BitBlt(hdc,
		x - OUTER_PIECE_WIDTH / 2,
		y - OUTER_PIECE_HEIGHT / 2,
		x + OUTER_PIECE_WIDTH / 2,
		y + OUTER_PIECE_HEIGHT / 2,
		_pieceHdc, 0, 0, SRCCOPY);
}

bool GameUI::launchEndGame(const bool &showMessage)
{
	GameStatus gameStatus = _gameSet->getStatus();
	if (gameStatus == GameStatus::BLACK_WIN)
	{
		if (showMessage)MessageBox(NULL, L"Black Won!", L"Game Ended", MB_OK);
		return true;
	}
	else if (gameStatus == GameStatus::WHITE_WIN)
	{
		if (showMessage)MessageBox(NULL, L"White Won!", L"Game Ended", MB_OK);
		return true;
	}
	else if (gameStatus == GameStatus::FIFTY_MOVE)
	{
		if (showMessage)MessageBox(NULL, L"Fifty moves were played with no capture, Draw!", L"Game Ended", MB_OK);
		return true;
	}
	else if (gameStatus == GameStatus::NO_LEGAL_MOVE)
	{
		if (showMessage)MessageBox(NULL, L"There is no legal move to play, but the king is not in check, Draw!", L"Game Ended", MB_OK);
		return true;
	}
	else if (gameStatus == GameStatus::THREEFOLD_REPETITION)
	{
		if (showMessage)MessageBox(NULL, L"The board was repeted threefold, Draw!", L"Game Ended", MB_OK);
		return true;
	}
	return false;
}

void GameUI::selectRightPieceBitmap(HDC &hdc, const Piece* piece)
{
	PieceType pieceType = piece->pieceType();
	if (piece->isWhite())
	{
		if (pieceType == PieceType::BISHOP)SelectObject(hdc, _whiteBishopBitmap);
		if (pieceType == PieceType::KING)SelectObject(hdc, _whiteKingBitmap);
		if (pieceType == PieceType::KNIGHT)SelectObject(hdc, _whiteKnightBitmap);
		if (pieceType == PieceType::PAWN)SelectObject(hdc, _whitePawnBitmap);
		if (pieceType == PieceType::QUEEN)SelectObject(hdc, _whiteQueenBitmap);
		if (pieceType == PieceType::ROOK)SelectObject(hdc, _whiteRookBitmap);
	}
	else
	{
		if (pieceType == PieceType::BISHOP)SelectObject(hdc, _blackBishopBitmap);
		if (pieceType == PieceType::KING)SelectObject(hdc, _blackKingBitmap);
		if (pieceType == PieceType::KNIGHT)SelectObject(hdc, _blackKnightBitmap);
		if (pieceType == PieceType::PAWN)SelectObject(hdc, _blackPawnBitmap);
		if (pieceType == PieceType::QUEEN)SelectObject(hdc, _blackQueenBitmap);
		if (pieceType == PieceType::ROOK)SelectObject(hdc, _blackRookBitmap);
	}
}

GameUI::~GameUI()
{
	DeleteObject(_backGroundBitmap);
	if (_gameSet)delete _gameSet;
	if(_artificial)delete _artificial;
	delete _randomGenerator;
	DeleteObject(white);
	DeleteObject(darkBrown);
	DeleteObject(green);
	DeleteObject(blue);
	DeleteObject(paleBrown);
	DeleteObject(black);
	DeleteObject(red);

	DeleteObject(_whiteBishopBitmap);
	DeleteObject(_whiteKingBitmap);
	DeleteObject(_whiteKnightBitmap);
	DeleteObject(_whitePawnBitmap);
	DeleteObject(_whiteQueenBitmap);
	DeleteObject(_whiteRookBitmap);
	DeleteObject(_blackBishopBitmap);
	DeleteObject(_blackKingBitmap);
	DeleteObject(_blackKnightBitmap);
	DeleteObject(_blackPawnBitmap);
	DeleteObject(_blackQueenBitmap);
	DeleteObject(_blackRookBitmap);

	if (_hdc)ReleaseDC(_hwnd, _hdc);
	if (_boardMovingHdc)ReleaseDC(_hwnd, _boardMovingHdc);
	if (_boardAndTokenHdc)ReleaseDC(_hwnd, _boardAndTokenHdc);
	if (_boardHdc)ReleaseDC(_hwnd, _boardHdc);
	if (_pieceHdc)ReleaseDC(_hwnd, _pieceHdc);
	if (_boardAndTokenBitmap)DeleteObject(_boardAndTokenBitmap);
	if (_boardBitmap)DeleteObject(_boardBitmap);
	if (_boardMovingBitmap)DeleteObject(_boardMovingBitmap);

	BitBoardMoveConstants::destroy();
}

const float GameUI::RECT_WIDTH = (WIDTH - 20) / (float)Board::NUMBER_COLUMN;
const float GameUI::RECT_HEIGHT = (HEIGHT - 120) / (float)Board::NUMBER_ROW;
const unsigned short GameUI::INNER_RECT_WIDTH = (unsigned short)(GameUI::RECT_WIDTH*0.9);
const unsigned short GameUI::INNER_RECT_HEIGHT = (unsigned short)(GameUI::RECT_HEIGHT*0.9);
const unsigned short GameUI::OUTER_PIECE_WIDTH = (unsigned short)(45);
const unsigned short GameUI::OUTER_PIECE_HEIGHT = (unsigned short)(45);