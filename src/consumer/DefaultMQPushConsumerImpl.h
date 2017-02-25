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

#ifndef __DEFAULTMQPUSHCONSUMERIMPL_H__
#define __DEFAULTMQPUSHCONSUMERIMPL_H__

#include <string>
#include <set>
#include <map>

#include "MQConsumerInner.h"
#include "MessageExt.h"
#include "QueryResult.h"
#include "ServiceState.h"
#include "PullResult.h"
#include "ConsumeMessageHook.h"
#include "MixAll.h"
#include "PullCallback.h"
#include "TimerThread.h"

namespace rmq
{
    class DefaultMQPushConsumer;
    class ConsumeMessageHook;
    class OffsetStore;
    class RebalanceImpl;
    class ConsumerStatManager;
    class ConsumeMessageService;
    class MessageListener;
    class PullRequest;
    class MQClientFactory;
    class PullAPIWrapper;
    class PullMessageService;
    class DefaultMQPushConsumerImplCallback;
	class MQException;

    /**
    * Push方式的Consumer实现
    *
    */
    class DefaultMQPushConsumerImpl : public  MQConsumerInner
    {
    public:
        DefaultMQPushConsumerImpl(DefaultMQPushConsumer* pDefaultMQPushConsumer);
		~DefaultMQPushConsumerImpl();

        void start();
        void suspend();
        void resume();
        void shutdown();
        bool isPause();
        void setPause(bool pause);

        bool hasHook();
        void registerHook(ConsumeMessageHook* pHook);
        void executeHookBefore(ConsumeMessageContext& context);
        void executeHookAfter(ConsumeMessageContext& context);

        void createTopic(const std::string& key, const std::string& newTopic, int queueNum);
        std::set<MessageQueue>* fetchSubscribeMessageQueues(const std::string& topic);

        long long earliestMsgStoreTime(const MessageQueue& mq);
        long long maxOffset(const MessageQueue& mq);
        long long minOffset(const MessageQueue& mq);
        OffsetStore* getOffsetStore() ;
        void setOffsetStore(OffsetStore* pOffsetStore);

        //MQConsumerInner
        std::string groupName() ;
        MessageModel messageModel() ;
        ConsumeType consumeType();
        ConsumeFromWhere consumeFromWhere();
        std::set<SubscriptionData> subscriptions();
        void doRebalance() ;
        void persistConsumerOffset() ;
        void updateTopicSubscribeInfo(const std::string& topic, const std::set<MessageQueue>& info);
        std::map<std::string, SubscriptionData>& getSubscriptionInner() ;
        bool isSubscribeTopicNeedUpdate(const std::string& topic);

        MessageExt* viewMessage(const std::string& msgId);
        QueryResult queryMessage(const std::string& topic,
                                 const std::string&  key,
                                 int maxNum,
                                 long long begin,
                                 long long end);

        void registerMessageListener(MessageListener* pMessageListener);
        long long searchOffset(const MessageQueue& mq, long long timestamp);
        void sendMessageBack(MessageExt& msg, int delayLevel, const std::string& brokerName);

        void subscribe(const std::string& topic, const std::string& subExpression);
        void unsubscribe(const std::string& topic);

        void updateConsumeOffset(MessageQueue& mq, long long offset);
        void updateCorePoolSize(int corePoolSize);
        bool isConsumeOrderly();
        void setConsumeOrderly(bool consumeOrderly);

        RebalanceImpl* getRebalanceImpl() ;
        MessageListener* getMessageListenerInner();
        DefaultMQPushConsumer* getDefaultMQPushConsumer() ;
        ConsumerStatManager* getConsumerStatManager();

    private:
        /**
        * 通过Tag过滤时，会存在offset不准确的情况，需要纠正
        */
        void correctTagsOffset(PullRequest& pullRequest) ;

        void pullMessage(PullRequest* pPullRequest);

        /**
        * 立刻执行这个PullRequest
        */
        void executePullRequestImmediately(PullRequest* pullRequest);

        /**
        * 稍后再执行这个PullRequest
        */
        void executePullRequestLater(PullRequest* pullRequest, long timeDelay);
		void executeTaskLater(kpr::TimerHandler* handler, long timeDelay);

        void makeSureStateOK();
        void checkConfig();
        void copySubscription() ;
        void updateTopicSubscribeInfoWhenSubscriptionChanged();

    private:
        static long long s_PullTimeDelayMillsWhenException;// 拉消息异常时，延迟一段时间再拉
        static long long s_PullTimeDelayMillsWhenFlowControl;
        static long long s_PullTimeDelayMillsWhenSuspend;

        static long long s_BrokerSuspendMaxTimeMillis;// 长轮询模式，Consumer连接在Broker挂起最长时间
        static long long s_ConsumerTimeoutMillisWhenSuspend;// 长轮询模式，Consumer超时时间（必须要大于brokerSuspendMaxTimeMillis）

        long long flowControlTimes1;
        long long flowControlTimes2;
        ServiceState m_serviceState;
        volatile bool m_pause;// 是否暂停接收消息 suspend/resume
        bool m_consumeOrderly;// 是否顺序消费消息
        DefaultMQPushConsumer* m_pDefaultMQPushConsumer;
        MQClientFactory* m_pMQClientFactory;
        PullAPIWrapper* m_pPullAPIWrapper;
        MessageListener* m_pMessageListenerInner;// 消费消息监听器
        OffsetStore* m_pOffsetStore;// 消费进度存储
        RebalanceImpl* m_pRebalanceImpl;// Rebalance实现
        ConsumerStatManager* m_pConsumerStatManager;
        ConsumeMessageService* m_pConsumeMessageService;// 消费消息服务

        std::list<ConsumeMessageHook*> m_hookList;//消费每条消息会回调
        friend class PullMessageService;
        friend class RebalancePushImpl;
        friend class DefaultMQPushConsumerImplCallback;
    };
}

#endif

