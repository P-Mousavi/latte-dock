/*
 * Copyright 2020  Michail Vourlakos <mvourlakos@gmail.com>
 *
 * This file is part of Latte-Dock
 *
 * Latte-Dock is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * Latte-Dock is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "screengeometries.h"

//!local
#include "../../lattecorona.h"
#include "../../screenpool.h"
#include "../../view/view.h"
#include "../../layout/genericlayout.h"
#include "../../layouts/manager.h"

// Qt
#include <QDebug>
#include <QtDBus>


#define LATTESERVICE "org.kde.lattedock"
#define PLASMASERVICE "org.kde.plasmashell"
#define PLASMASTRUTNAMESPACE "org.kde.PlasmaShell.StrutManager"

namespace Latte {
namespace PlasmaExtended {

ScreenGeometries::ScreenGeometries(Latte::Corona *parent)
    : QObject(parent),
      m_corona(parent)
{
    m_startupInitTimer.setInterval(5000);
    m_startupInitTimer.setSingleShot(true);
    connect(&m_startupInitTimer, &QTimer::timeout, this, &ScreenGeometries::init);

    m_publishTimer.setInterval(1000);
    m_publishTimer.setSingleShot(true);
    connect(&m_publishTimer, &QTimer::timeout, this, &ScreenGeometries::updateGeometries);

    m_startupInitTimer.start();
}

ScreenGeometries::~ScreenGeometries()
{
    qDebug() << "Plasma Extended Screen Geometries :: Deleted...";
}

void ScreenGeometries::init()
{
    QDBusInterface plasmaStrutsIface(PLASMASERVICE, "/StrutManager", PLASMASTRUTNAMESPACE, QDBusConnection::sessionBus());



    if (plasmaStrutsIface.isValid()) {
        m_plasmaInterfaceAvailable = true;

        qDebug() << " PLASMA STRUTS MANAGER :: is available...";

        connect(m_corona, &Latte::Corona::availableScreenRectChangedFrom, this, &ScreenGeometries::availableScreenGeometryChangedFrom);
        connect(m_corona, &Latte::Corona::availableScreenRegionChangedFrom, this, &ScreenGeometries::availableScreenGeometryChangedFrom);

        connect(m_corona->layoutsManager(), &Latte::Layouts::Manager::currentLayoutNameChanged, this, [&]() {
            m_publishTimer.start();
        });

        m_publishTimer.start();
    }
}

bool ScreenGeometries::screenIsActive(const QString &screenName) const
{
    for (QScreen *screen : qGuiApp->screens()) {
        if (screen->name() == screenName) {
            return true;
        }
    }

    return false;
}

void ScreenGeometries::updateGeometries()
{
    if (!m_plasmaInterfaceAvailable) {
        return;
    }

    QDBusInterface plasmaStrutsIface(PLASMASERVICE, "/StrutManager", PLASMASTRUTNAMESPACE, QDBusConnection::sessionBus());

    if (!plasmaStrutsIface.isValid()) {
        return;
    }

    QStringList availableScreenNames;

    //! check for available geometries changes
    for (QScreen *screen : qGuiApp->screens()) {
        QString scrName = screen->name();
        int scrId = m_corona->screenPool()->id(screen->name());

        if (m_corona->screenPool()->hasId(scrId)) {
            QRect availableRect = m_corona->availableScreenRectWithCriteria(scrId,
                                                                            QString(),
                                                                            m_acceptedModes,
                                                                            QList<Plasma::Types::Location>());

            QRegion availableRegion = m_corona->availableScreenRegionWithCriteria(scrId,
                                                                                  QString(),
                                                                                  m_acceptedModes,
                                                                                  QList<Plasma::Types::Location>());

            if (!m_lastAvailableRect.contains(scrName) || m_lastAvailableRect[scrName] != availableRect) {
                m_lastAvailableRect[scrName] = availableRect;
                plasmaStrutsIface.call("setAvailableScreenRect", LATTESERVICE, scrName, availableRect);
                qDebug() << " PLASMA SCREEN GEOMETRIES AVAILABLE RECT :: " << screen->name() << " : " << availableRect;
            }

            if (!m_lastAvailableRegion.contains(scrName) || m_lastAvailableRegion[scrName] != availableRegion) {
                m_lastAvailableRegion[scrName] = availableRegion;
                plasmaStrutsIface.call("setAvailableScreenRegion", LATTESERVICE, scrName, availableRegion);
                qDebug() << " PLASMA SCREEN GEOMETRIES AVAILABLE REGION :: " << screen->name() << " : " << availableRegion;
            }
        }

        availableScreenNames << scrName;
    }

    //! check for inactive screens that were published previously
    for (QString &lastScrName : m_lastScreenNames) {
        if (!screenIsActive(lastScrName)) {
            //! screen became inactive and its geometries could be unpublished
            plasmaStrutsIface.call("setAvailableScreenRect", LATTESERVICE, lastScrName, QRect());
            plasmaStrutsIface.call("setAvailableScreenRegion", LATTESERVICE, lastScrName, QRegion());
        }
    }

    m_lastScreenNames = availableScreenNames;
}

void ScreenGeometries::availableScreenGeometryChangedFrom(Latte::View *origin)
{
    if (origin && origin->layout() && origin->layout()->isCurrent()) {
        m_publishTimer.start();
    }
}

}
}
