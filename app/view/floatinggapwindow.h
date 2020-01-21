/*
*  Copyright 2020 Michail Vourlakos <mvourlakos@gmail.com>
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

#ifndef FLOATINGGAPWINDOW_H
#define FLOATINGWINDOW_H

// local
#include "../lattecorona.h"
#include "../wm/windowinfowrap.h"

// Qt
#include <QObject>
#include <QQuickView>
#include <QTimer>

namespace KWayland {
namespace Client {
class PlasmaShellSurface;
}
}

namespace Latte {
class Corona;
class View;
}

namespace Latte {
namespace ViewPart {

//! What is the importance of this class?
//!
//! This window is responsible to identify if the mouse is still present
//! in the REAL FLOATING GAP between the VIEW and the SCREEN EDGE.
//! When VIEWS are REAL FLOATING then the VIEW Window is really placed
//! as it is shown to the user. In that case we need a way to check
//! where the mouse is even though it is OUTSIDE the VIEW or the
//! SCREENEDGEGHOSTWINDOW. The main functionality of FloatingGapWindow
//! is that it is TEMPORARILY shown/draw after a MUSTHIDE signal of
//! VisibilityManager was sent; in order to check if the mouse is still
//! inside the FLOATINGGAP. After it has really identified where mouse
//! is present, an FloatingGapWindow::asyncContainsMouse(contains) signal
//! is sent.

class FloatingGapWindow : public QQuickView
{
    Q_OBJECT

public:
    FloatingGapWindow(Latte::View *view);
    ~FloatingGapWindow() override;

    int location();
    int thickness() const;

    void hideWithMask();
    void showWithMask();

    Latte::View *parentView();

    KWayland::Client::PlasmaShellSurface *surface();

    void callAsyncContainsMouse();

signals:
    void asyncContainsMouseChanged(bool contains); //called from visibility to check if mouse is in the free sensitive floating area
    void dragEntered();
    void forcedShown(); //[workaround] forced shown to avoid a KWin issue that hides windows when activities are stopped

protected:
    bool event(QEvent *ev) override;

private slots:
    void startGeometryTimer();
    void updateGeometry();
    void fixGeometry();

private:
    bool containsMouse() const;
    void setContainsMouse(bool contains);
    void setupWaylandIntegration();

    void triggerAsyncContainsMouseSignals();

private:
    bool m_debugMode{false};

    bool m_containsMouse{false};
    bool m_inDelete{false};

    bool m_inAsyncContainsMouse{false}; //called from visibility to check if mouse is in the free sensitive floating area

    int m_thickness{2};

    QRect m_calculatedGeometry;

    //! [workaround] colors in order to help masking to apply immediately
    //! for some reason when the window in with no content the mask is not
    //! update immediately
    QColor m_hideColor;
    QColor m_showColor;

    QTimer m_asyncMouseTimer; //called from visibility to check if mouse is in the free sensitive floating area
    QTimer m_fixGeometryTimer;

    //! HACK: Timers in order to handle KWin faulty
    //! behavior that hides Views when closing Activities
    //! with no actual reason
    QTimer m_visibleHackTimer1;
    QTimer m_visibleHackTimer2;
    //! Connections for the KWin visibility hack
    QList<QMetaObject::Connection> connectionsHack;

    Latte::View *m_latteView{nullptr};

    QPointer<Latte::Corona> m_corona;

    Latte::WindowSystem::WindowId m_trackedWindowId;
    KWayland::Client::PlasmaShellSurface *m_shellSurface{nullptr};
};

}
}
#endif
