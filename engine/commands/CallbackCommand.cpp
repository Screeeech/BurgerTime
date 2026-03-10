//
// Created by lily-laptop on 10/03/2026.
//

#include "CallbackCommand.h"

#include <utility>

dae::CallbackCommand::CallbackCommand(std::function<void()> callback)
    : m_callback(std::move(callback))
{
}

void dae::CallbackCommand::Execute()
{
    m_callback();
}