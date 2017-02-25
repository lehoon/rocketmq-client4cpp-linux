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
#ifndef __REBALANCEIMPL_H__
#define __REBALANCEIMPL_H__

#include <map>
#include <string>
#include <set>
#include <list>

#include "ConsumeType.h"
#include "MessageQueue.h"
#include "ProcessQueue.h"
#include "PullRequest.h"
#include "SubscriptionData.h"

namespace rmq
{
    class AllocateMessageQueueStrategy;
    class MQClientFactory;

    /**
    * Rebalance的具体实现
    *
    */
    class RebalanceImpl
    {
    public:
        RebalanceImpl(const std::string& consumerGroup,
                      MessageModel messageModel,
                      AllocateMessageQueueStrategy* pAllocateMessageQueueStrategy,
                      MQClientFactory* pMQClientFactory);
        virtual ~RebalanceImpl();

        virtual void messageQueueChanged(const std::string& topic,
                                         std::set<MessageQueue>& mqAll,
                                         std::set<MessageQueue>& mqDivided) = 0;
        virtual bool removeUnnecessaryMessageQueue(MessageQueue& mq, ProcessQueue& pq) = 0;
        virtual void dispatchPullRequest(std::list<PullRequest*>& pullRequestList) = 0;
        virtual long long computePullFromWhere(MessageQueue& mq) = 0;
		virtual ConsumeType consumeType() = 0;

        bool lock(MessageQueue& mq);
        void lockAll();

        void unlock(MessageQueue& mq, bool oneway);
        void unlockAll(bool oneway);

        void doRebalance();

        std::map<std::string, SubscriptionData>& getSubscriptionInner();
        std::map<MessageQueue, ProcessQueue*>& getProcessQueueTable();
        std::map<std::string, std::set<MessageQueue> >& getTopicSubscribeInfoTable();

        std::string& getConsumerGroup();
        void setConsumerGroup(const std::string& consumerGroup);

        MessageModel getMessageModel();
        void setMessageModel(MessageModel messageModel);

        AllocateMessageQueueStrategy* getAllocateMessageQueueStrategy();
        void setAllocateMessageQueueStrategy(AllocateMessageQueueStrategy* pAllocateMessageQueueStrategy);

        MQClientFactory* getmQClientFactory();
        void setmQClientFactory(MQClientFactory* pMQClientFactory);

		void removeProcessQueue(const MessageQueue& mq);

    private:
        std::map<std::string, std::set<MessageQueue> > buildProcessQueueTableByBrokerName();
        void rebalanceByTopic(const std::string& topic);
        bool updateProcessQueueTableInRebalance(const std::string& topic, std::set<MessageQueue>& mqSet);
        void truncateMessageQueueNotMyTopic();

    protected:
        // 分配好的队列，消息存储也在这里
        std::map<MessageQueue, ProcessQueue*> m_processQueueTable;
        kpr::Mutex m_processQueueTableLock;

        // 可以订阅的所有队列（定时从Name Server更新最新版本）
        std::map<std::string, std::set<MessageQueue> > m_topicSubscribeInfoTable;
        kpr::Mutex m_topicSubscribeInfoTableLock;//todo: 这个信息在运行时可能会变化，所以需要加锁

        // 订阅关系，用户配置的原始数据 key <topic> value <SubscriptionData>
        std::map<std::string, SubscriptionData> m_subscriptionInner;
        kpr::Mutex m_subscriptionInnerLock;//todo: 订阅关系在初始化时就确定了，所以这里不需要加锁

        std::string m_consumerGroup;
        MessageModel m_messageModel;
        AllocateMessageQueueStrategy* m_pAllocateMessageQueueStrategy;
        MQClientFactory* m_pMQClientFactory;
    };
}

#endif
