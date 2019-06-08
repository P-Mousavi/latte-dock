/*
*  Copyright 2019  Michail Vourlakos <mvourlakos@gmail.com>
*
*  This file is part of Latte-Dock
*
*  Latte-Dock is free software; you can redistribute it and/or
*  modify it under the terms of the GNU General Public License as
*  published by the Free Software Foundation; either version 2 of
*  the License, or (at your option) any later version.
*
*  Latte-Dock is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "currentscreentracker.h"

// local
#include "../view.h"
#include "../../wm/schemecolors.h"
#include "../../wm/tracker/lastactivewindow.h"
#include "../../wm/tracker/trackerwindows.h"

namespace Latte {
namespace ViewPart {
namespace TrackerPart {

CurrentScreenTracker::CurrentScreenTracker(WindowsTracker *parent)
    : QObject(parent),
      m_latteView(parent->view()),
      m_wm(parent->wm())
{
    init();

    m_wm->windowsTracker()->addView(m_latteView);
}

CurrentScreenTracker::~CurrentScreenTracker()
{
    m_wm->windowsTracker()->removeView(m_latteView);
}

void  CurrentScreenTracker::init()
{
    connect(m_wm->windowsTracker(), &WindowSystem::Tracker::Windows::informationAnnounced, this, [&](const Latte::View *view) {
        if (m_latteView == view) {
            initSignalsForInformation();
        }
    });

    connect(m_wm->windowsTracker(), &WindowSystem::Tracker::Windows::enabledChanged, this, [&](const Latte::View *view) {
        if (m_latteView == view) {
            emit enabledChanged();
        }
    });

    connect(m_wm->windowsTracker(), &WindowSystem::Tracker::Windows::activeWindowMaximizedChanged, this, [&](const Latte::View *view) {
        if (m_latteView == view) {
            emit activeWindowMaximizedChanged();
        }
    });

    connect(m_wm->windowsTracker(), &WindowSystem::Tracker::Windows::activeWindowTouchingChanged, this, [&](const Latte::View *view) {
        if (m_latteView == view) {
            emit activeWindowTouchingChanged();
        }
    });

    connect(m_wm->windowsTracker(), &WindowSystem::Tracker::Windows::existsWindowActiveChanged, this, [&](const Latte::View *view) {
        if (m_latteView == view) {
            emit existsWindowActiveChanged();
        }
    });

    connect(m_wm->windowsTracker(), &WindowSystem::Tracker::Windows::existsWindowMaximizedChanged, this, [&](const Latte::View *view) {
        if (m_latteView == view) {
            emit existsWindowMaximizedChanged();
        }
    });

    connect(m_wm->windowsTracker(), &WindowSystem::Tracker::Windows::existsWindowTouchingChanged, this, [&](const Latte::View *view) {
        if (m_latteView == view) {
            emit existsWindowTouchingChanged();
        }
    });

    connect(m_wm->windowsTracker(), &WindowSystem::Tracker::Windows::activeWindowSchemeChanged, this, [&](const Latte::View *view) {
        if (m_latteView == view) {
            emit activeWindowSchemeChanged();
        }
    });

    connect(m_wm->windowsTracker(), &WindowSystem::Tracker::Windows::touchingWindowSchemeChanged, this, [&](const Latte::View *view) {
        if (m_latteView == view) {
            emit touchingWindowSchemeChanged();
        }
    });
}

void CurrentScreenTracker::initSignalsForInformation()
{
    connect(lastActiveWindow(), &WindowSystem::Tracker::LastActiveWindow::draggingStarted,
            this, &CurrentScreenTracker::activeWindowDraggingStarted);

    emit lastActiveWindowChanged();
}

bool CurrentScreenTracker::activeWindowMaximized() const
{
    return m_wm->windowsTracker()->activeWindowMaximized(m_latteView);
}

bool CurrentScreenTracker::activeWindowTouching() const
{
    return m_wm->windowsTracker()->activeWindowTouching(m_latteView);
}

bool CurrentScreenTracker::existsWindowActive() const
{
    return m_wm->windowsTracker()->existsWindowActive(m_latteView);
}

bool CurrentScreenTracker::existsWindowMaximized() const
{
    return m_wm->windowsTracker()->existsWindowMaximized(m_latteView);
}

bool CurrentScreenTracker::existsWindowTouching() const
{
    return m_wm->windowsTracker()->existsWindowTouching(m_latteView);
}

WindowSystem::SchemeColors *CurrentScreenTracker::activeWindowScheme() const
{
    return m_wm->windowsTracker()->activeWindowScheme(m_latteView);
}

WindowSystem::SchemeColors *CurrentScreenTracker::touchingWindowScheme() const
{
    return m_wm->windowsTracker()->touchingWindowScheme(m_latteView);
}

bool CurrentScreenTracker::enabled() const
{
    return m_wm->windowsTracker()->enabled(m_latteView);
}

void CurrentScreenTracker::setEnabled(bool active)
{
    m_wm->windowsTracker()->setEnabled(m_latteView, active);
}

WindowSystem::Tracker::LastActiveWindow *CurrentScreenTracker::lastActiveWindow()
{
    return m_wm->windowsTracker()->lastActiveWindow(m_latteView);
}


//! Window Functions
void CurrentScreenTracker::requestMoveLastWindow(int localX, int localY)
{
    m_wm->windowsTracker()->lastActiveWindow(m_latteView)->requestMove(m_latteView, localX, localY);
}

}
}
}