#include "MpCommandWrapper.h"

// WARNING : Auto-generated file, changes made will disappear when re-generated.

#include <iostream>

#include "MpActionCommand.h"
#include "MpAttackCommand.h"
#include "MpAttackMoveCommand.h"
#include "MpCancelCommand.h"
#include "MpMoveCommand.h"
#include "MpPointTargetActionCommand.h"
#include "MpStopCommand.h"
#include "MpUnitTargetActionCommand.h"

void MpCommandWrapper::serialize(std::ostream& os) const
{
	if (this->command)
	{
		switch (this->command->getChildTypeIndex())
		{
			case 0:
			{
				os.put(0);
				os.write((char*)this->command.get() + sizeof(char*), sizeof(MpActionCommand) - sizeof(char*));
				break;
			}
			case 1:
			{
				os.put(1);
				os.write((char*)this->command.get() + sizeof(char*), sizeof(MpAttackCommand) - sizeof(char*));
				break;
			}
			case 2:
			{
				os.put(2);
				os.write((char*)this->command.get() + sizeof(char*), sizeof(MpAttackMoveCommand) - sizeof(char*));
				break;
			}
			case 3:
			{
				os.put(3);
				os.write((char*)this->command.get() + sizeof(char*), sizeof(MpCancelCommand) - sizeof(char*));
				break;
			}
			case 4:
			{
				os.put(4);
				os.write((char*)this->command.get() + sizeof(char*), sizeof(MpCommand) - sizeof(char*));
				break;
			}
			case 5:
			{
				os.put(5);
				os.write((char*)this->command.get() + sizeof(char*), sizeof(MpMoveCommand) - sizeof(char*));
				break;
			}
			case 6:
			{
				os.put(6);
				os.write((char*)this->command.get() + sizeof(char*), sizeof(MpPointTargetActionCommand) - sizeof(char*));
				break;
			}
			case 7:
			{
				os.put(7);
				os.write((char*)this->command.get() + sizeof(char*), sizeof(MpStopCommand) - sizeof(char*));
				break;
			}
			case 8:
			{
				os.put(8);
				os.write((char*)this->command.get() + sizeof(char*), sizeof(MpUnitTargetActionCommand) - sizeof(char*));
				break;
			}
		}
	}
	else
	{
		os.put(0xff);
	}
}

void MpCommandWrapper::deserialize(std::istream& is)
{
	switch (is.get())
	{
		case 0:
		{
			this->command = std::make_unique<MpActionCommand>();
			is.read((char*)this->command.get() + sizeof(char*), sizeof(MpActionCommand) - sizeof(char*));
			break;
		}
		case 1:
		{
			this->command = std::make_unique<MpAttackCommand>();
			is.read((char*)this->command.get() + sizeof(char*), sizeof(MpAttackCommand) - sizeof(char*));
			break;
		}
		case 2:
		{
			this->command = std::make_unique<MpAttackMoveCommand>();
			is.read((char*)this->command.get() + sizeof(char*), sizeof(MpAttackMoveCommand) - sizeof(char*));
			break;
		}
		case 3:
		{
			this->command = std::make_unique<MpCancelCommand>();
			is.read((char*)this->command.get() + sizeof(char*), sizeof(MpCancelCommand) - sizeof(char*));
			break;
		}
		case 4:
		{
			this->command = std::make_unique<MpCommand>();
			is.read((char*)this->command.get() + sizeof(char*), sizeof(MpCommand) - sizeof(char*));
			break;
		}
		case 5:
		{
			this->command = std::make_unique<MpMoveCommand>();
			is.read((char*)this->command.get() + sizeof(char*), sizeof(MpMoveCommand) - sizeof(char*));
			break;
		}
		case 6:
		{
			this->command = std::make_unique<MpPointTargetActionCommand>();
			is.read((char*)this->command.get() + sizeof(char*), sizeof(MpPointTargetActionCommand) - sizeof(char*));
			break;
		}
		case 7:
		{
			this->command = std::make_unique<MpStopCommand>();
			is.read((char*)this->command.get() + sizeof(char*), sizeof(MpStopCommand) - sizeof(char*));
			break;
		}
		case 8:
		{
			this->command = std::make_unique<MpUnitTargetActionCommand>();
			is.read((char*)this->command.get() + sizeof(char*), sizeof(MpUnitTargetActionCommand) - sizeof(char*));
			break;
		}
		default:
			break;
	}
}

