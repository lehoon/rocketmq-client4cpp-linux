/**
 * Copyright (C) 2013 kangliqiang ,kangliq@163.com
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef __LOCALTRANSACTIONEXECUTER_H__
#define __LOCALTRANSACTIONEXECUTER_H__

#include "SendResult.h"

namespace rmq
{
	/**
	 * 执行本地事务，由客户端回调
	 *
	 */
	class LocalTransactionExecuter
	{
	public:
	    virtual~LocalTransactionExecuter() {}
	    virtual LocalTransactionState executeLocalTransactionBranch(Message& msg, void* arg) = 0;
	};
}

#endif
