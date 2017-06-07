#include "ComMultisampleState.hpp"
#include "ComLogger.hpp"

namespace CastorCom
{
	static const Castor::String ERROR_UNINITIALISED_STATE = cuT( "The multisample state must be initialised" );

	CMultisampleState::CMultisampleState()
	{
	}

	CMultisampleState ::~CMultisampleState()
	{
	}
}