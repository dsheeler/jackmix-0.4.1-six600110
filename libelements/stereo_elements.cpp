/*
    Copyright 2004 - 2007 Arnold Krille <arnold@arnoldarts.de>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation;
    version 2 of the License.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.

*/

#include "stereo_elements.h"
#include "stereo_elements.moc"

#include "knob.h"
#include "slider.h"

#include "shortcutmanager.h"

#include <QtGui/QLayout>
#include <QtGui/QPushButton>
#include <QtCore/QList>
#include <QtGui/QLabel>
#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QWidget>

using namespace JackMix;
using namespace JackMix::MixerElements;
using namespace JackMix::MixingMatrix;

/// The Factory for creating this things...
class StereoFactory : public JackMix::MixingMatrix::ElementFactory
{
public:
    StereoFactory() : ElementFactory() { globaldebug(); m_sm = ShortcutManager::instance(); }
    ~StereoFactory() {}

    QStringList canCreate() const {
        return QStringList()<<"Mono2StereoElement"<<"Stereo2StereoElement";
    }
    QStringList canCreate( int in, int out ) const {
        if ( in==1 && out==2 ) return QStringList()<<"Mono2StereoElement";
        if ( in==2 && out==2 ) return QStringList()<<"Stereo2StereoElement";
        return QStringList();
    }

    Element* create( QString type , QStringList ins, QStringList outs, Widget* p, const char* n=0 ) {
        Stereo2StereoElement *tmp;
        if ( type=="Mono2StereoElement" )
            return new Mono2StereoElement( ins, outs, p, n );
        if ( type=="Stereo2StereoElement" ) {
            tmp = new Stereo2StereoElement( ins, outs, p, n );
            if (ins[0] == "xmms_l" && ins[1] == "xmms_r" && outs[0] == "out_1" && outs[1] == "out_2") {
                qDebug("i am connecting a stereo2stereo element to the volume controls");
                m_sm->connect (tmp, "globalshortcuts/increase_vol", "Ctrl+Shift+Up", SLOT (increase_volume ()), true);
                m_sm->connect (tmp, "globalshortcuts/decrease_vol", "Ctrl+Shift+Down", SLOT (decrease_volume ()), true);
            } else if (ins[0] == "xmms_l" && ins[1] == "xmms_r" && outs[0] == "aux_l" && outs[1] == "aux_r") {
                m_sm->connect (tmp, "globalshortcuts/increase_vol_xmms_aux", "Ctrl+Alt+Up", SLOT(increase_volume()), true);
                m_sm->connect (tmp, "globalshortcuts/decrease_vol_xmms_aux", "Ctrl+Alt+Down", SLOT(decrease_volume()), true);
            } else if (ins[0] == "in_1" && ins[1] == "in_2" && outs[0] == "aux_l" && outs[1] == "aux_r") {
                m_sm->connect (tmp, "globalshortcuts/increase_vol_in_1_2_aux", "Ctrl+Alt+o", SLOT(increase_volume()), true);
                m_sm->connect (tmp, "globalshortcuts/decrease_vol_in_1_2_aux", "Ctrl+Alt+i", SLOT(decrease_volume()), true);
            } else if (ins[0] == "in_1" && ins[1] == "in_2" && outs[0] == "out_1" && outs[1] == "out_2") {
                qDebug("i am connecting a stereo2stereo element to the volume controls");
                m_sm->connect (tmp, "globalshortcuts/increase_vol_in_1_2_out", "Ctrl+Shift+o", SLOT (increase_volume ()), true);
                m_sm->connect (tmp, "globalshortcuts/decrease_vol_in_1_2_out", "Ctrl+Shift+i", SLOT (decrease_volume ()), true);
            } else if (ins[0] == "guitar_l" && ins[1] == "guitar_r" && outs[0] == "out_1" && outs[1] == "out_2") {
                qDebug("i am connecting a stereo2stereo element to the volume controls");
                m_sm->connect (tmp, "globalshortcuts/increase_vol_in_guitar_out", "Ctrl+Shift+k", SLOT (increase_volume ()), true);
                m_sm->connect (tmp, "globalshortcuts/decrease_vol_in_guitar_out", "Ctrl+Shift+j", SLOT (decrease_volume ()), true);
            } else if (ins[0] == "guitar_l" && ins[1] == "guitar_r" && outs[0] == "aux_l" && outs[1] == "aux_r") {
                qDebug("i am connecting a stereo2stereo element to the volume controls");
                m_sm->connect (tmp, "globalshortcuts/increase_vol_in_guitar_aux", "Ctrl+Alt+k", SLOT (increase_volume ()), true);
                m_sm->connect (tmp, "globalshortcuts/decrease_vol_in_guitar_aux", "Ctrl+Alt+j", SLOT (decrease_volume ()), true);
            } else if (ins[0] == "return_l" && ins[1] == "return_r" && outs[0] == "out_1" && outs[1] == "out_2") {
                qDebug("i am connecting a stereo2stereo element to the volume controls");
                m_sm->connect (tmp, "globalshortcuts/increase_vol_in_return_out", "Ctrl+Shift+e", SLOT (increase_volume ()), true);
                m_sm->connect (tmp, "globalshortcuts/decrease_vol_in_return_out", "Ctrl+Shift+w", SLOT (decrease_volume ()), true);
            } else if (ins[0] == "return_l" && ins[1] == "return_r" && outs[0] == "aux_l" && outs[1] == "aux_r") {
                qDebug("i am connecting a stereo2stereo element to the volume controls");
                m_sm->connect (tmp, "globalshortcuts/increase_vol_in_return_aux", "Ctrl+Alt+e", SLOT (increase_volume ()), true);
                m_sm->connect (tmp, "globalshortcuts/decrease_vol_in_return_aux", "Ctrl+Alt+w", SLOT (decrease_volume ()), true);
            }
            return tmp;
        }
        return 0;
    }
private:
    ShortcutManager *m_sm;

};

void MixerElements::init_stereo_elements() {
	new StereoFactory();
}


Mono2StereoElement::Mono2StereoElement( QStringList inchannel, QStringList outchannels, MixingMatrix::Widget* p, const char* n )
	: Element( inchannel, outchannels, p, n )
	, dB2VolCalc( -42, 6 )
	, _inchannel( inchannel[ 0 ] )
	, _outchannel1( outchannels[ 0 ] )
	, _outchannel2( outchannels[ 1 ] )
	, _balance_value( 0 )
	, _volume_value( 0 )
{
	//qDebug( "Mono2StereoElement::Mono2StereoElement()" );
	double left = backend()->getVolume( _inchannel, _outchannel1 );
	double right = backend()->getVolume( _inchannel, _outchannel2 );
	//qDebug( " volumes: %f, %f", left, right );
	if ( left>right ) {
		_volume_value = left;
		_balance_value = right-left;
	} else {
		_volume_value = right;
		_balance_value = right-left;
	}
	//qDebug( " values: %f, %f", _volume_value, _balance_value );
	QVBoxLayout* _layout = new QVBoxLayout( this );
	_layout->setMargin( 0 );
	_layout->setSpacing( 0 );

	menu()->addAction( "Select", this, SLOT( slot_simple_select() ) );
	menu()->addAction( "Replace", this, SLOT( slot_simple_replace() ) );

	_balance = new JackMix::GUI::Knob( _balance_value, -1, 1, 2, 0.1, this, "%1" );
	_layout->addWidget( _balance, 10 );
	connect( _balance, SIGNAL( valueChanged( double ) ), this, SLOT( balance( double ) ) );
	_volume = new JackMix::GUI::Slider( amptodb( _volume_value ), dbmin, dbmax, 2, 3, this );
	_layout->addWidget( _volume, 20 );
	connect( _volume, SIGNAL( valueChanged( double ) ), this, SLOT( volume( double ) ) );
}
Mono2StereoElement::~Mono2StereoElement() {
}


void Mono2StereoElement::balance( double n ) {
	//qDebug( "Mono2StereoElement::balance( double %f )", n );
	_balance_value = n;
	calculateVolumes();
	_balance->value( n );
	emit valueChanged( this, QString( "balance" ) );
}
void Mono2StereoElement::volume( double n ) {
	//qDebug( "Mono2StereoElement::volume( double %f )", n );
	_volume_value = n;
	calculateVolumes();
	_volume->value( n );
	emit valueChanged( this, QString( "volume" ) );
}

void Mono2StereoElement::calculateVolumes() {
	double left, right;
		left = dbtoamp( _volume_value );
		right = dbtoamp( _volume_value );
	if ( _balance_value > 0 )
		left = dbtoamp( _volume_value )*( 1-_balance_value );
	if ( _balance_value < 0 )
		right = dbtoamp( _volume_value )*( 1+_balance_value );
	backend()->setVolume( _inchannel, _outchannel1, left );
	backend()->setVolume( _inchannel, _outchannel2, right );
}




Stereo2StereoElement::Stereo2StereoElement( QStringList inchannels, QStringList outchannels, MixingMatrix::Widget* p, const char* n )
	: Element( inchannels, outchannels, p, n )
	, dB2VolCalc( -42, 6 )
	, _inchannel1( inchannels[ 0 ] )
	, _inchannel2( inchannels[ 1 ] )
	, _outchannel1( outchannels[ 0 ] )
	, _outchannel2( outchannels[ 1 ] )
	, _balance_value( 0 )
	, _volume_value( 0 )
{
	backend()->setVolume( _inchannel1, _outchannel2, 0 );
	backend()->setVolume( _inchannel2, _outchannel1, 0 );
	double left = backend()->getVolume( _inchannel1, _outchannel1 );
	double right = backend()->getVolume( _inchannel2, _outchannel2 );
	if ( left>right )
		_volume_value = left;
	else
		_volume_value = right;
	_balance_value = qMin( qMax( right-left, -1.0 ), 1.0 );
	QGridLayout* _layout = new QGridLayout( this );
	_layout->setSpacing( 1 );
	_layout->setMargin( 2 );
	_balance_widget = new JackMix::GUI::Slider( _balance_value, -1, 1, 2, 0.1, this, "%1" );
	_layout->addWidget( _balance_widget, 0,0 );
	_layout->setRowStretch( 0, 0 );
	connect( _balance_widget, SIGNAL( valueChanged( double ) ), this, SLOT( balance( double ) ) );
	_volume_widget = new JackMix::GUI::Slider( amptodb( _volume_value ), dbmin, dbmax, 1, 3, this );
	_layout->addWidget( _volume_widget, 1,0 );
	_layout->setRowStretch( 1, 255 );
	connect( _volume_widget, SIGNAL( valueChanged( double ) ), this, SLOT( volume( double ) ) );
        _volume_value = amptodb(_volume_value);

	QAction *toggle = new QAction( "Toggle Selection", this );
	connect( toggle, SIGNAL( triggered() ), this, SLOT( slot_simple_select() ) );
	menu()->addAction( toggle );
	QAction *replace = new QAction( "Replace", this );
	connect( replace, SIGNAL( triggered() ), this, SLOT( slot_simple_replace() ) );
	menu()->addAction( replace );



}
Stereo2StereoElement::~Stereo2StereoElement() {
}

void Stereo2StereoElement::increase_volume() {
    qDebug( "Stereo2StereoElement::increase_volume()" );

    //qDebug() << this->_volume_value;
    this->volume(this->_volume_value + 3.0);

}

void Stereo2StereoElement::decrease_volume() {
    qDebug( "Stereo2StereoElement::decrease_volume()" );

    //  qDebug() << this->_volume_value;
    this->volume(this->_volume_value - 3.0);
}

void Stereo2StereoElement::balance( double n ) {
	//qDebug( "Mono2StereoElement::balance( double %f )", n );
	_balance_value = n;
	_balance_widget->value( n );
	calculateVolumes();
	emit valueChanged( this, QString( "balance" ) );
}

void Stereo2StereoElement::volume( double n ) {
	//qDebug( "Mono2StereoElement::volume( double %f )", n );
	_volume_value = n;
	_volume_widget->value( n );
	calculateVolumes();
	emit valueChanged( this, QString( "volume" ) );
}

void Stereo2StereoElement::calculateVolumes() {
	double left, right;
		left = dbtoamp( _volume_value );
		right = dbtoamp( _volume_value );
	if ( _balance_value > 0 )
		left = dbtoamp( _volume_value )*( 1-_balance_value );
	if ( _balance_value < 0 )
		right = dbtoamp( _volume_value )*( 1+_balance_value );
	backend()->setVolume( _inchannel1, _outchannel1, left );
	backend()->setVolume( _inchannel2, _outchannel2, right );
}
