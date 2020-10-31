#include "utils.h"
#ifdef max
#undef max
#define needReDefine
#endif // max
int GetOntInt(std::istream& inStream, std::ostream& outStream, int numMin, int numMax)
{
	char skiped, getChar;
	int result;
	bool inputedFlag, negativeFlag, syntaxErrorFlag;

	while (true)
	{
		inputedFlag = false;
		negativeFlag = false;
		syntaxErrorFlag = false;
		result = 0;

		skiped = inStream.get();
		if (skiped == '-')
			negativeFlag = true;
		else
			inStream.unget();
		while (true && !syntaxErrorFlag)//get int part
		{
			getChar = inStream.get();
			if (getChar >= '0' && getChar <= '9')//Input is number
			{
				inputedFlag = true;
				result = result * 10 + getChar - '0';
			}
			else//Input is other char
			{
				if (getChar != '\n')
					syntaxErrorFlag = true;
				else
					inStream.unget();
				break;
			}
		}

		if (syntaxErrorFlag)
			outStream << "Input SyntaxError! Please Try it Again!" << std::endl;
		else
		{
			if (inputedFlag)
			{
				result = negativeFlag ? -result : result;
				if (result < numMin)
				{
					outStream << "Input Int Too Small!Please Try It Again!" << std::endl;
				}
				else
				{
					if (result > numMax)
					{
						outStream << "Input Int Too Large!Please Try It Again!" << std::endl;
					}
					else
					{
						inStream.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
						return result;
					}
				}
			}
			else
			{
				outStream << "Nothing Number Input!Please Try It Again!" << std::endl;
			}
		}
		inStream.clear();
		inStream.sync();
		inStream.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	}
}

void ClearInStream(std::istream& inStream)
{
	inStream.clear();
	inStream.sync();
	inStream.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

#ifdef needReDefine
#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif
#undef needReDefine
#endif