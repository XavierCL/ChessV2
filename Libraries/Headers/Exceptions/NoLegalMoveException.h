#ifndef NO_LEGAL_MOVE_EXCEPTION_H
#define NO_LEGAL_MOVE_EXCEPTION_H

#include <exception>

class NoLegalMoveException: public std::exception
{
	virtual const char* what() const throw() override;
};

#endif