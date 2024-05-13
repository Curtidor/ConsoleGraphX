#pragma once
#include "component.h"
#include "entity.h"

namespace ConsoleGraphX
{
	struct Script : public ConsoleGraphX_Interal::Component
	{
	protected:
		bool _m_isEnabled;

	public:
		Script();
		Script(const Script& script);

		virtual void Awake(Entity* owner);
		virtual void Start(Entity* owner);
		virtual void Update(Entity* owner);

		bool IsEnabled();

		void SetState(bool state);

		ConsoleGraphX_Interal::Component* Clone() const override;
		int GetID() const override;

	};
};

