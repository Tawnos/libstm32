/*
 * event_bus.h
 *
 *  Created on: Sep 17, 2017
 *      Author: cmdc0de
 */

#ifndef LIBSTM32_OBSERVER_EVENT_BUS_H_
#define LIBSTM32_OBSERVER_EVENT_BUS_H_

#include <stdint.h>
#include <typeinfo>
#include "../etl/src/map.h"
#include "../etl/src/pool.h"
#include "../etl/src/list.h"

namespace cmdc0de {

class SignalRegistration {
public:
	SignalRegistration() {
	}
	virtual void call(void *subject, const void *event)=0;
	virtual ~SignalRegistration() {
	}
	virtual intptr_t getSubjectKey() const =0;
	virtual intptr_t getEventKey() const =0;
	virtual bool compare(const SignalRegistration *r) const =0;
private:
};

//bool ::operator==(SignalRegistration *s1, SignalRegistration *s2){
//	return s1->compare(s2);
//}

template<typename ObserverType, typename EventType, typename SubjectType>
class TypedSignalRegistration: public SignalRegistration {
public:
	TypedSignalRegistration(ObserverType *observer, bool isOneShot) :
			SignalRegistration(), Observer(observer) {

	}
	virtual void call(void *subject, const void *event) {
		Observer->receiveSignal(static_cast<SubjectType>(subject),
				static_cast<const EventType*>(event));
	}
	virtual ~TypedSignalRegistration() {
	}
	virtual uint32_t getSubjectKey() const {
		return typeid(SubjectType).hash_code();
	}
	virtual uint32_t getEventKey() const {
		return typeid(EventType).hash_code();
	}
	virtual bool compare(const SignalRegistration *r) const {
		TypedSignalRegistration *tr = static_cast<TypedSignalRegistration*>(r);
		return tr->Observer == Observer
				&& getSubjectKey() == tr->getSubjectKey()
				&& tr->getEventKey() == getEventKey();
	}
private:
	ObserverType *Observer;
};

//Map<eventHash,Map<subjectHash,List<SignalRegistration*> > >
template<int MAX_NUM_EVENTS, int MAX_EVENT_TYPES, int MAX_SUBJECTS,
		int MAX_LISTENERS_PER_EVENT_PER_SUBJECT = 3>
class EventBus {
public:
	typedef etl::list<SignalRegistration*, MAX_LISTENERS_PER_EVENT_PER_SUBJECT> SIGNAL_LIST;
	typedef typename SIGNAL_LIST::iterator SIGNAL_LIST_IT;
	typedef etl::map<uint32_t, SIGNAL_LIST, MAX_SUBJECTS> MAP_SUB_TO_LISTENER;
	typedef typename MAP_SUB_TO_LISTENER::iterator MAP_SUB_TO_LISTENER_IT;
	//event it to map of subjext to listeners
	typedef etl::map<uint32_t, MAP_SUB_TO_LISTENER, MAX_EVENT_TYPES> EMAP;
	typedef typename EMAP::iterator EMAP_IT;
public:
	EventBus() :
			BusMap() {
	}
	template<typename ObserverT, typename EventT, typename SubjectT>
	void addListener(ObserverT *o, EventT *e, SubjectT *s) {
		addListener(o, e, s, false);
	}
	template<typename ObserverT, typename EventT, typename SubjectT>
	void addListener(ObserverT *o, EventT *e, SubjectT *s, bool isOneShot) {
		TypedSignalRegistration<ObserverT, EventT, SubjectT> *tsr = new (
				Pool.allocate()) TypedSignalRegistration<ObserverT, EventT,
				SubjectT>(o, isOneShot);
		//TODO protect against double add
		//TODO IMPL oneShot
		std::pair<EMAP_IT, bool> p = BusMap.insert(std::make_pair(tsr->getEventKey(),EMAP::mapped_type()));
				std::pair<MAP_SUB_TO_LISTENER_IT, bool> p2 =
						p.first.second.insert(
								std::make_pair(tsr->getSubjectKey(),
										SIGNAL_LIST::mapped_type()));
				p2.first.second.push_back(tsr);

			}
			template<typename ObserverT, typename EventT, typename SubjectT>
			void removeListener(ObserverT *o, EventT *e, SubjectT *s) {
				TypedSignalRegistration<ObserverT, EventT, SubjectT> tsr(o,
						false);
				EMAP_IT it = BusMap.find(tsr.getEventKey());
				if (it != BusMap.end()) {
					MAP_SUB_TO_LISTENER_IT sit =
							EMAP::iteratorBusMap.find(tsr.getSubjectKey());
					if (sit != (*it).end()) {
						(*sit).erase(&tsr);
					}
				}

				Pool.release(&tsr);
			}
			template<typename T, typename E>
			void emitSignal(T *subject, const E &event) {
				EMAP_IT em = BusMap.find(typeid(event).hash_code());
				MAP_SUB_TO_LISTENER_IT it = em.second.find(
						typeid(subject).hash_code());
				SIGNAL_LIST_IT sit = it.second.begin();
				for (; sit != it.second.end(); ++sit) {
					(*sit)->call(subject, &event);
				}
			}

		private:
			//just using the size of the TypedSignalRegistration
			etl::pool<uint64_t, MAX_NUM_EVENTS> Pool;

			EMAP BusMap;
		};

		}
#endif /* LIBSTM32_OBSERVER_EVENT_BUS_H_ */
