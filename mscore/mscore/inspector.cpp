//=============================================================================
//  MuseScore
//  Music Composition & Notation
//  $Id:$
//
//  Copyright (C) 2011 Werner Schweer
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License version 2
//  as published by the Free Software Foundation and appearing in
//  the file LICENSE.GPL
//=============================================================================

#include "inspector.h"
#include "inspectorBeam.h"
#include "inspectorGroupElement.h"
#include "musescore.h"
#include "libmscore/element.h"
#include "libmscore/score.h"
#include "libmscore/box.h"
#include "libmscore/undo.h"
#include "libmscore/spacer.h"
#include "libmscore/note.h"
#include "libmscore/chord.h"
#include "libmscore/segment.h"
#include "libmscore/rest.h"
#include "libmscore/beam.h"
#include "libmscore/clef.h"
#include "libmscore/notedot.h"
#include "libmscore/hook.h"
#include "libmscore/stem.h"

//---------------------------------------------------------
//   showInspector
//---------------------------------------------------------

void MuseScore::showInspector(bool visible)
      {
      QAction* a = getAction("inspector");
      if (visible) {
            if (!inspector) {
                  inspector = new Inspector();
                  connect(inspector, SIGNAL(inspectorVisible(bool)), a, SLOT(setChecked(bool)));
                  addDockWidget(Qt::RightDockWidgetArea, inspector);
                  }
            if (cs)
                  selectionChanged(cs->selection().state());
            }
      if (inspector)
            inspector->setVisible(visible);
      a->setChecked(visible);
      }

//---------------------------------------------------------
//   Inspector
//---------------------------------------------------------

Inspector::Inspector(QWidget* parent)
   : QDockWidget(tr("Inspector"), parent)
      {
      setObjectName("inspector");
      setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
      QScrollArea* sa = new QScrollArea;
      sa->setWidgetResizable(true);
      QWidget* mainWidget = new QWidget;
      mainWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding); //??
      setWidget(sa);
      sa->setWidget(mainWidget);

      layout = new QVBoxLayout;
      mainWidget->setLayout(layout);
      ie        = 0;
      _element  = 0;
      QHBoxLayout* hbox = new QHBoxLayout;
      apply = new QPushButton;
      apply->setText(tr("Apply"));
      apply->setEnabled(false);
      hbox->addWidget(apply);
      layout->addStretch(10);
      layout->addLayout(hbox);
      connect(apply, SIGNAL(clicked()), SLOT(applyClicked()));
      }

//---------------------------------------------------------
//   closeEvent
//---------------------------------------------------------

void Inspector::closeEvent(QCloseEvent* ev)
      {
      emit inspectorVisible(false);
      QWidget::closeEvent(ev);
      }

//---------------------------------------------------------
//   reset
//---------------------------------------------------------

void Inspector::reset()
      {
      if (ie)
            ie->setElement(_element);
      apply->setEnabled(false);
      }

//---------------------------------------------------------
//   applyClicked
//---------------------------------------------------------

void Inspector::applyClicked()
      {
      if (ie)
            ie->apply();
      apply->setEnabled(false);
      }

//---------------------------------------------------------
//   setElement
//---------------------------------------------------------

void Inspector::setElement(Element* e)
      {
      if (e == 0 || _element == 0 || (e->type() != _element->type())) {
            delete ie;
            ie = 0;
            _element = e;
            apply->setEnabled(_element != 0);

            if (_element == 0)
                  return;
            switch(_element->type()) {
                  case FBOX:
                  case TBOX:
                  case VBOX:         ie = new InspectorVBox(this); break;
                  case HBOX:         ie = new InspectorHBox(this); break;
                  case ARTICULATION: ie = new InspectorArticulation(this); break;
                  case SPACER:       ie = new InspectorSpacer(this); break;
                  case NOTE:         ie = new InspectorNote(this); break;
                  case REST:         ie = new InspectorRest(this); break;
                  case CLEF:         ie = new InspectorClef(this); break;
                  case BEAM:         ie = new InspectorBeam(this); break;
                  default:           ie = new InspectorElement(this); break;
                  }
            layout->insertWidget(0, ie);
            }
      _element = e;
      ie->setElement(_element);
      apply->setVisible(true);
      apply->setEnabled(false);
      }

//---------------------------------------------------------
//   setElementList
//---------------------------------------------------------

void Inspector::setElementList(const QList<Element*>& el)
      {
      delete ie;
      ie = new InspectorGroupElement(this);
      layout->insertWidget(0, ie);
      _element = 0;
      _el = el;
      apply->setVisible(false);
      apply->setEnabled(false);
      }

//---------------------------------------------------------
//   InspectorElementElement
//---------------------------------------------------------

InspectorElementElement::InspectorElementElement(QWidget* parent)
   : QWidget(parent)
      {
      setupUi(this);
      connect(color,        SIGNAL(colorChanged(QColor)), SLOT(colorChanged(QColor)));
      connect(offsetX,      SIGNAL(valueChanged(double)), SLOT(offsetXChanged(double)));
      connect(offsetY,      SIGNAL(valueChanged(double)), SLOT(offsetYChanged(double)));
      connect(visible,      SIGNAL(stateChanged(int)),    SLOT(visibleChanged(int)));
      connect(resetColor,   SIGNAL(clicked()), SLOT(resetColorClicked()));
      connect(resetX,       SIGNAL(clicked()), SLOT(resetXClicked()));
      connect(resetY,       SIGNAL(clicked()), SLOT(resetYClicked()));
      connect(resetVisible, SIGNAL(clicked()), SLOT(resetVisibleClicked()));
      }

//---------------------------------------------------------
//   dirty
//---------------------------------------------------------

bool InspectorElementElement::dirty() const
      {
      qreal _spatium = e->score()->spatium();
      return offsetX->value()     != (e->pos().x() / _spatium)
         ||  offsetY->value()     != (e->pos().y() / _spatium)
         ||  color->color()       != e->color()
         ||  visible->isChecked() != e->visible();
      }

//---------------------------------------------------------
//   setElement
//---------------------------------------------------------

void InspectorElementElement::setElement(Element* element)
      {
      e = element;
      elementName->setText(e->name());
      color->setColor(e->color());
      qreal _spatium = e->score()->spatium();
      offsetX->setValue(e->pos().x() / _spatium);
      offsetY->setValue(e->pos().y() / _spatium);
      resetColor->setEnabled(e->color() != MScore::defaultColor);
      resetX->setEnabled(e->userOff().x() != 0.0);
      resetY->setEnabled(e->userOff().y() != 0.0);
      visible->blockSignals(true);
      visible->setChecked(e->visible());
      visible->blockSignals(false);
      resetVisible->setEnabled(!e->visible());
      }

//---------------------------------------------------------
//   visibleChanged
//---------------------------------------------------------

void InspectorElementElement::visibleChanged(int)
      {
      resetVisible->setEnabled(!visible->isChecked());
      emit enableApply();
      }

//---------------------------------------------------------
//   colorChanged
//---------------------------------------------------------

void InspectorElementElement::colorChanged(QColor)
      {
      resetColor->setEnabled(color->color() != MScore::defaultColor);
      emit enableApply();
      }

//---------------------------------------------------------
//   offsetXChanged
//---------------------------------------------------------

void InspectorElementElement::offsetXChanged(double)
      {
      resetX->setEnabled(offsetX->value() != e->ipos().x());
      emit enableApply();
      }

//---------------------------------------------------------
//   offsetYChanged
//---------------------------------------------------------

void InspectorElementElement::offsetYChanged(double)
      {
      resetY->setEnabled(offsetY->value() != e->ipos().y());
      emit enableApply();
      }

//---------------------------------------------------------
//   resetColorClicked
//---------------------------------------------------------

void InspectorElementElement::resetColorClicked()
      {
      color->setColor(MScore::defaultColor);
      resetColor->setEnabled(false);
      emit enableApply();
      }

//---------------------------------------------------------
//   resetXClicked
//---------------------------------------------------------

void InspectorElementElement::resetXClicked()
      {
      qreal _spatium = e->score()->spatium();
      offsetX->setValue(e->ipos().x() / _spatium);
      resetX->setEnabled(false);
      emit enableApply();
      }

//---------------------------------------------------------
//   resetVisibleClicked
//---------------------------------------------------------

void InspectorElementElement::resetVisibleClicked()
      {
      visible->setChecked(true);
      resetVisible->setEnabled(false);
      emit enableApply();
      }

//---------------------------------------------------------
//   resetTrailingSpace
//---------------------------------------------------------

void InspectorElementElement::resetYClicked()
      {
      qreal _spatium = e->score()->spatium();
      offsetY->setValue(e->ipos().y() / _spatium);
      resetY->setEnabled(false);
      emit enableApply();
      }

//---------------------------------------------------------
//   apply
//---------------------------------------------------------

void InspectorElementElement::apply()
      {
      Score* score    = e->score();
      qreal _spatium  = score->spatium();
      QPointF o(offsetX->value() * _spatium, offsetY->value() * _spatium);
      if (o != e->pos())
            score->undoChangeUserOffset(e, o - e->ipos());
      if (e->color() != color->color())
            score->undoChangeProperty(e, P_COLOR, color->color());
      if (e->visible() != visible->isChecked())
            score->undoChangeProperty(e, P_VISIBLE, visible->isChecked());
      }

//---------------------------------------------------------
//   InspectorElement
//---------------------------------------------------------

InspectorElement::InspectorElement(QWidget* parent)
   : InspectorBase(parent)
      {
      ie = new InspectorElementElement(this);
      layout->addWidget(ie);
      connect(ie, SIGNAL(enableApply()), inspector, SLOT(enableApply()));
      }

//---------------------------------------------------------
//   setElement
//---------------------------------------------------------

void InspectorElement::setElement(Element* e)
      {
      ie->setElement(e);
      }

//---------------------------------------------------------
//   apply
//---------------------------------------------------------

void InspectorElement::apply()
      {
      Element* e = inspector->element();
      Score* score = e->score();
      score->startCmd();
      ie->apply();
      score->setLayoutAll(true);
      score->endCmd();
      mscore->endCmd();
      }

//---------------------------------------------------------
//   InspectorVBox
//---------------------------------------------------------

InspectorVBox::InspectorVBox(QWidget* parent)
   : InspectorBase(parent)
      {
      QWidget* w = new QWidget;
      vb.setupUi(w);
      layout->addWidget(w);

      iList[0].t = P_TOP_GAP;
      iList[0].w = vb.topGap;
      iList[0].r = vb.resetTopGap;

      iList[1].t = P_BOTTOM_GAP;
      iList[1].w = vb.bottomGap;
      iList[1].r = vb.resetBottomGap;

      iList[2].t = P_LEFT_MARGIN;
      iList[2].w = vb.leftMargin;
      iList[2].r = vb.resetLeftMargin;

      iList[3].t = P_RIGHT_MARGIN;
      iList[3].w = vb.rightMargin;
      iList[3].r = vb.resetRightMargin;

      iList[4].t = P_TOP_MARGIN;
      iList[4].w = vb.topMargin;
      iList[4].r = vb.resetTopMargin;

      iList[5].t = P_BOTTOM_MARGIN;
      iList[5].w = vb.bottomMargin;
      iList[5].r = vb.resetBottomMargin;

      iList[6].t = P_BOX_HEIGHT;
      iList[6].w = vb.height;
      iList[6].r = 0;

      mapSignals();
      }

//---------------------------------------------------------
//   InspectorHBox
//---------------------------------------------------------

InspectorHBox::InspectorHBox(QWidget* parent)
   : InspectorBase(parent)
      {
      QWidget* w = new QWidget;
      hb.setupUi(w);
      layout->addWidget(w);

      iList[0].t = P_TOP_GAP;
      iList[0].w = hb.leftGap;
      iList[0].r = hb.resetLeftGap;

      iList[1].t = P_BOTTOM_GAP;
      iList[1].w = hb.rightGap;
      iList[1].r = hb.resetRightGap;

      iList[2].t = P_BOX_WIDTH;
      iList[2].w = hb.width;
      iList[2].r = 0;

      mapSignals();
      }

//---------------------------------------------------------
//   InspectorArticulation
//---------------------------------------------------------

InspectorArticulation::InspectorArticulation(QWidget* parent)
   : InspectorBase(parent)
      {
      QWidget* w = new QWidget;

      ar.setupUi(w);
      layout->addWidget(w);
      connect(ar.x, SIGNAL(valueChanged(double)), inspector, SLOT(enableApply()));
      connect(ar.y, SIGNAL(valueChanged(double)), inspector, SLOT(enableApply()));
      connect(ar.direction, SIGNAL(currentIndexChanged(int)), inspector, SLOT(enableApply()));
      connect(ar.anchor, SIGNAL(currentIndexChanged(int)), inspector, SLOT(enableApply()));
      }

//---------------------------------------------------------
//   setElement
//---------------------------------------------------------

void InspectorArticulation::setElement(Element* e)
      {
      Articulation* a = static_cast<Articulation*>(e);
      qreal _spatium = e->score()->spatium();
      ar.elementName->setText(e->name());
      ar.x->blockSignals(true);
      ar.y->blockSignals(true);
      ar.direction->blockSignals(true);
      ar.anchor->blockSignals(true);

      ar.x->setValue(a->pos().x() / _spatium);
      ar.y->setValue(a->pos().y() / _spatium);
      ar.direction->setCurrentIndex(int(a->direction()));
      ar.anchor->setCurrentIndex(int(a->anchor()));

      ar.x->blockSignals(false);
      ar.y->blockSignals(false);
      ar.direction->blockSignals(false);
      ar.anchor->blockSignals(false);
      }

//---------------------------------------------------------
//   apply
//---------------------------------------------------------

void InspectorArticulation::apply()
      {
      Articulation* a = static_cast<Articulation*>(inspector->element());
      Score* score    = a->score();
      qreal _spatium  = score->spatium();

      QPointF o(ar.x->value() * _spatium, ar.y->value() * _spatium);
      score->startCmd();
      if (o != a->pos())
            score->undoChangeUserOffset(a, o - a->ipos());
      Direction d = Direction(ar.direction->currentIndex());
      ArticulationAnchor anchor = ArticulationAnchor(ar.anchor->currentIndex());
      if (anchor != a->anchor())
            score->undoChangeProperty(a, P_ARTICULATION_ANCHOR, int(anchor));
      if (d != a->direction())
            score->undoChangeProperty(a, P_DIRECTION, int(d));
      score->endCmd();
      mscore->endCmd();
      }

//---------------------------------------------------------
//   InspectorSpacer
//---------------------------------------------------------

InspectorSpacer::InspectorSpacer(QWidget* parent)
   : InspectorBase(parent)
      {
      QWidget* w = new QWidget;

      sp.setupUi(w);
      layout->addWidget(w);
      connect(sp.height, SIGNAL(valueChanged(double)), inspector, SLOT(enableApply()));
      }

//---------------------------------------------------------
//   setElement
//---------------------------------------------------------

void InspectorSpacer::setElement(Element* e)
      {
      Spacer* spacer = static_cast<Spacer*>(e);
      sp.elementName->setText(e->name());
      sp.height->setValue(spacer->gap() / spacer->spatium());
      }

//---------------------------------------------------------
//   apply
//---------------------------------------------------------

void InspectorSpacer::apply()
      {
      Spacer* spacer = static_cast<Spacer*>(inspector->element());
      Score* score   = spacer->score();
      qreal space    = sp.height->value() * spacer->spatium();
      if (space != spacer->gap()) {
            score->startCmd();
            //TODO make undoable
            spacer->setGap(space);
            score->setLayoutAll(true);
            score->setDirty(true);

            score->endCmd();
            mscore->endCmd();
            }
      }

//---------------------------------------------------------
//   InspectorSegment
//---------------------------------------------------------

InspectorSegment::InspectorSegment(QWidget* parent)
   : QWidget(parent)
      {
      setupUi(this);
      connect(leadingSpace,       SIGNAL(valueChanged(double)), SLOT(leadingSpaceChanged(double)));
      connect(trailingSpace,      SIGNAL(valueChanged(double)), SLOT(trailingSpaceChanged(double)));
      connect(resetLeadingSpace,  SIGNAL(clicked()), SLOT(resetLeadingSpaceClicked()));
      connect(resetTrailingSpace, SIGNAL(clicked()), SLOT(resetTrailingSpaceClicked()));
      }

//---------------------------------------------------------
//   dirty
//---------------------------------------------------------

bool InspectorSegment::dirty() const
      {
      return segment->extraLeadingSpace().val() != leadingSpace->value()
         || segment->extraTrailingSpace().val() != trailingSpace->value();
      }

//---------------------------------------------------------
//   setElement
//---------------------------------------------------------

void InspectorSegment::setElement(Segment* s)
      {
      segment = s;
      leadingSpace->setValue(segment->extraLeadingSpace().val());
      trailingSpace->setValue(segment->extraTrailingSpace().val());
      resetLeadingSpace->setEnabled(leadingSpace->value() != 0.0);
      resetTrailingSpace->setEnabled(leadingSpace->value() != 0.0);
      }

//---------------------------------------------------------
//   leadingSpaceChanged
//---------------------------------------------------------

void InspectorSegment::leadingSpaceChanged(double)
      {
      resetLeadingSpace->setEnabled(leadingSpace->value() != 0.0);
      emit enableApply();
      }

//---------------------------------------------------------
//   trailingSpaceChanged
//---------------------------------------------------------

void InspectorSegment::trailingSpaceChanged(double)
      {
      resetTrailingSpace->setEnabled(trailingSpace->value() != 0.0);
      emit enableApply();
      }

//---------------------------------------------------------
//   resetLeadingSpace
//---------------------------------------------------------

void InspectorSegment::resetLeadingSpaceClicked()
      {
      leadingSpace->setValue(0.0);
      }

//---------------------------------------------------------
//   resetTrailingSpace
//---------------------------------------------------------

void InspectorSegment::resetTrailingSpaceClicked()
      {
      trailingSpace->setValue(0.0);
      }

//---------------------------------------------------------
//   apply
//---------------------------------------------------------

void InspectorSegment::apply()
      {
      qreal val = leadingSpace->value();
      if (segment->extraLeadingSpace().val() != val)
            segment->score()->undoChangeProperty(segment, P_LEADING_SPACE, val);
      val = trailingSpace->value();
      if (segment->extraTrailingSpace().val() != val)
            segment->score()->undoChangeProperty(segment, P_TRAILING_SPACE, val);
      }

static const int heads[] = {
      HEAD_NORMAL, HEAD_CROSS, HEAD_DIAMOND, HEAD_TRIANGLE,
      HEAD_SLASH, HEAD_XCIRCLE, HEAD_DO, HEAD_RE, HEAD_MI, HEAD_FA, HEAD_SOL, HEAD_LA, HEAD_TI,
      HEAD_BREVIS_ALT
      };

//---------------------------------------------------------
//   InspectorNoteBase
//---------------------------------------------------------

InspectorNoteBase::InspectorNoteBase(QWidget* parent)
   : QWidget(parent)
      {
      setupUi(this);
      //
      // fix order of note heads
      //
      for (int i = 0; i < HEAD_GROUPS; ++i) {
            noteHeadGroup->setItemData(i, QVariant(heads[i]));
            }
      connect(small,              SIGNAL(stateChanged(int)),        SLOT(smallChanged(int)));
      connect(mirrorHead,         SIGNAL(currentIndexChanged(int)), SLOT(mirrorHeadChanged(int)));
      connect(dotPosition,        SIGNAL(currentIndexChanged(int)), SLOT(dotPositionChanged(int)));
      connect(ontimeOffset,       SIGNAL(valueChanged(int)),        SLOT(ontimeOffsetChanged(int)));
      connect(offtimeOffset,      SIGNAL(valueChanged(int)),        SLOT(offtimeOffsetChanged(int)));
      connect(resetSmall,         SIGNAL(clicked()), SLOT(resetSmallClicked()));
      connect(resetMirrorHead,    SIGNAL(clicked()), SLOT(resetMirrorClicked()));
      connect(resetDotPosition,   SIGNAL(clicked()), SLOT(resetDotPositionClicked()));
      connect(resetOntimeOffset,  SIGNAL(clicked()), SLOT(resetOntimeOffsetClicked()));
      connect(resetOfftimeOffset, SIGNAL(clicked()), SLOT(resetOfftimeOffsetClicked()));

      connect(noteHeadGroup,      SIGNAL(currentIndexChanged(int)), SLOT(noteHeadGroupChanged(int)));
      connect(noteHeadType,       SIGNAL(currentIndexChanged(int)), SLOT(noteHeadTypeChanged(int)));
      connect(tuning,             SIGNAL(valueChanged(double)),     SLOT(tuningChanged(double)));
      connect(velocityType,       SIGNAL(currentIndexChanged(int)), SLOT(velocityTypeChanged(int)));
      connect(velocity,           SIGNAL(valueChanged(int)),        SLOT(velocityChanged(int)));

      connect(resetNoteHeadGroup, SIGNAL(clicked()), SLOT(resetNoteHeadGroupClicked()));
      connect(resetNoteHeadType,  SIGNAL(clicked()), SLOT(resetNoteHeadTypeClicked()));
      connect(resetTuning,        SIGNAL(clicked()), SLOT(resetTuningClicked()));
      connect(resetVelocityType,  SIGNAL(clicked()), SLOT(resetVelocityTypeClicked()));
      }

//---------------------------------------------------------
//   setElement
//---------------------------------------------------------

void InspectorNoteBase::setElement(Note* n)
      {
      _userVelocity = 0;
      _veloOffset   = 0;
      note          = n;

      small->setChecked(note->small());
      mirrorHead->setCurrentIndex(note->userMirror());
      dotPosition->setCurrentIndex(note->dotPosition());
      ontimeOffset->setValue(note->onTimeOffset());
      offtimeOffset->setValue(note->offTimeOffset());

      int headGroup = note->headGroup();
      int headGroupIndex = 0;
      for (int i = 0; i < HEAD_GROUPS; ++i) {
            noteHeadGroup->setItemData(i, QVariant(heads[i]));
            if (headGroup == heads[i])
                  headGroupIndex = i;
            }
      noteHeadGroup->setCurrentIndex(headGroupIndex);
      noteHeadType->setCurrentIndex(int(note->headType()));
      tuning->setValue(note->tuning());
      int val = note->veloOffset();
      velocity->setValue(val);
      velocityType->setCurrentIndex(int(note->veloType()));
      if (note->veloType() == USER_VAL)
            _userVelocity = val;
      else
            _veloOffset = val;

      resetSmall->setEnabled(note->small());
      resetMirrorHead->setEnabled(note->userMirror() != DH_AUTO);
      resetDotPosition->setEnabled(note->dotPosition() != AUTO);
      resetOntimeOffset->setEnabled(note->onTimeUserOffset());
      resetOfftimeOffset->setEnabled(note->offTimeUserOffset());
      }

//---------------------------------------------------------
//   dirty
//---------------------------------------------------------

bool InspectorNoteBase::dirty() const
      {
      return note->small()            != small->isChecked()
         || note->userMirror()        != mirrorHead->currentIndex()
         || note->dotPosition()       != dotPosition->currentIndex()
         || note->onTimeUserOffset()  != ontimeOffset->value()
         || note->offTimeUserOffset() != offtimeOffset->value()
         || note->headGroup()         != noteHeadGroup->itemData(noteHeadGroup->currentIndex())
         || note->headType()          != noteHeadType->currentIndex()
         || note->tuning()            != tuning->value()
         || note->veloOffset()        != velocity->value()
         || note->veloType()          != velocityType->currentIndex()
         ;
      }

//---------------------------------------------------------
//   apply
//---------------------------------------------------------

void InspectorNoteBase::apply()
      {
      Score* score = note->score();
      bool b = small->isChecked();
      if (note->small() != b)
            score->undoChangeProperty(note, P_SMALL, b);
      int val = mirrorHead->currentIndex();
      if (note->userMirror() != val)
            score->undoChangeProperty(note, P_MIRROR_HEAD, val);
      val = dotPosition->currentIndex();
      if (note->dotPosition() != val)
            score->undoChangeProperty(note, P_DOT_POSITION, val);
      val = ontimeOffset->value();
      if (note->onTimeOffset() != val)
            score->undoChangeProperty(note, P_ONTIME_OFFSET, val);
      val = offtimeOffset->value();
      if (note->offTimeOffset() != val)
            score->undoChangeProperty(note, P_OFFTIME_OFFSET, val);
      val = noteHeadGroup->itemData(noteHeadGroup->currentIndex()).toInt();
      if (note->headGroup() != val)
            score->undoChangeProperty(note, P_HEAD_GROUP, val);
      val = noteHeadType->currentIndex();
      if (note->headType() != val)
            score->undoChangeProperty(note, P_HEAD_TYPE, val);
      if (note->tuning() != tuning->value())
            score->undoChangeProperty(note, P_TUNING, tuning->value());
      if (note->veloOffset() != velocity->value())
            score->undoChangeProperty(note, P_VELO_OFFSET, velocity->value());
      if (note->veloType() != velocityType->currentIndex())
            score->undoChangeProperty(note, P_VELO_TYPE, velocityType->currentIndex());
      }

//---------------------------------------------------------
//   smallChanged
//---------------------------------------------------------

void InspectorNoteBase::smallChanged(int)
      {
      resetSmall->setEnabled(small->isChecked());
      emit enableApply();
      }

//---------------------------------------------------------
//   mirrorHeadChanged
//---------------------------------------------------------

void InspectorNoteBase::mirrorHeadChanged(int)
      {
      resetMirrorHead->setEnabled(note->userMirror() != DH_AUTO);
      emit enableApply();
      }

//---------------------------------------------------------
//   dotPositionChanged
//---------------------------------------------------------

void InspectorNoteBase::dotPositionChanged(int)
      {
      resetDotPosition->setEnabled(note->dotPosition() != AUTO);
      emit enableApply();
      }

//---------------------------------------------------------
//   ontimeOffsetChanged
//---------------------------------------------------------

void InspectorNoteBase::ontimeOffsetChanged(int)
      {
      resetOntimeOffset->setEnabled(note->onTimeUserOffset());
      emit enableApply();
      }

//---------------------------------------------------------
//   offtimeOffsetChanged
//---------------------------------------------------------

void InspectorNoteBase::offtimeOffsetChanged(int)
      {
      resetOfftimeOffset->setEnabled(note->offTimeUserOffset());
      emit enableApply();
      }

//---------------------------------------------------------
//   noteHeadGroupChanged
//---------------------------------------------------------

void InspectorNoteBase::noteHeadGroupChanged(int val)
      {
      resetNoteHeadGroup->setEnabled(val != 0);
      emit enableApply();
      }

//---------------------------------------------------------
//   noteHeadTypeChanged
//---------------------------------------------------------

void InspectorNoteBase::noteHeadTypeChanged(int val)
      {
      resetNoteHeadType->setEnabled(val != 0);
      emit enableApply();
      }

//---------------------------------------------------------
//   tuningChanged
//---------------------------------------------------------

void InspectorNoteBase::tuningChanged(double val)
      {
      resetTuning->setEnabled(val != 0.0);
      emit enableApply();
      }

//---------------------------------------------------------
//   velocityTypeChanged
//---------------------------------------------------------

void InspectorNoteBase::velocityTypeChanged(int val)
      {
      switch(val) {
            case USER_VAL:
                  velocity->setEnabled(true);
                  velocity->setSuffix("");
                  velocity->setRange(0, 127);
                  velocity->setValue(_userVelocity);
                  break;
            case OFFSET_VAL:
                  velocity->setEnabled(true);
                  velocity->setSuffix("%");
                  velocity->setRange(-200, 200);
                  velocity->setValue(_veloOffset);
                  break;
            }
      resetVelocityType->setEnabled(val != 0);
      emit enableApply();
      }

//---------------------------------------------------------
//   velocityChanged
//---------------------------------------------------------

void InspectorNoteBase::velocityChanged(int val)
      {
      if (velocityType->currentIndex() == USER_VAL)
            _userVelocity = val;
      else
            _veloOffset = val;
      emit enableApply();
      }

//---------------------------------------------------------
//   resetSmall
//---------------------------------------------------------

void InspectorNoteBase::resetSmallClicked()
      {
      small->setChecked(false);
      }

//---------------------------------------------------------
//   resetMirrorClicked
//---------------------------------------------------------

void InspectorNoteBase::resetMirrorClicked()
      {
      mirrorHead->setCurrentIndex(0);
      }

//---------------------------------------------------------
//   resetDotPositionClicked
//---------------------------------------------------------

void InspectorNoteBase::resetDotPositionClicked()
      {
      dotPosition->setCurrentIndex(0);
      }

//---------------------------------------------------------
//   resetOntimeOffsetClicked
//---------------------------------------------------------

void InspectorNoteBase::resetOntimeOffsetClicked()
      {
      ontimeOffset->setValue(0);
      }

//---------------------------------------------------------
//   resetOfftimeOffsetClicked
//---------------------------------------------------------

void InspectorNoteBase::resetOfftimeOffsetClicked()
      {
      offtimeOffset->setValue(0);
      }

//---------------------------------------------------------
//   resetNoteHeadGroupClicked
//---------------------------------------------------------

void InspectorNoteBase::resetNoteHeadGroupClicked()
      {
      noteHeadGroup->setCurrentIndex(0);
      }

//---------------------------------------------------------
//   resetNoteHeadTypeClicked
//---------------------------------------------------------

void InspectorNoteBase::resetNoteHeadTypeClicked()
      {
      noteHeadType->setCurrentIndex(0);
      }

//---------------------------------------------------------
//   resetTuningClicked
//---------------------------------------------------------

void InspectorNoteBase::resetTuningClicked()
      {
      tuning->setValue(0.0);
      }

//---------------------------------------------------------
//   resetVelocityTypeClicked
//---------------------------------------------------------

void InspectorNoteBase::resetVelocityTypeClicked()
      {
      velocityType->setCurrentIndex(0);
      }

//---------------------------------------------------------
//   InspectorNote
//---------------------------------------------------------

InspectorNote::InspectorNote(QWidget* parent)
   : InspectorBase(parent)
      {
      iElement = new InspectorElementElement(this);
      layout->addWidget(iElement);

      iNote    = new InspectorNoteBase(this);
      layout->addWidget(iNote);

      iChord = new InspectorChord(this);
      layout->addWidget(iChord);

      iSegment = new InspectorSegment(this);
      layout->addWidget(iSegment);

      layout->addSpacing(20);

      //
      // Select
      //
      QLabel* l = new QLabel;
      l->setText(tr("Select"));
      QFont font(l->font());
      font.setBold(true);
      l->setFont(font);
      l->setAlignment(Qt::AlignHCenter);
      layout->addWidget(l);
      QFrame* f = new QFrame;
      f->setFrameStyle(QFrame::HLine | QFrame::Raised);
      f->setLineWidth(2);
      layout->addWidget(f);

      QHBoxLayout* hbox = new QHBoxLayout;
      dot1 = new QToolButton(this);
      dot1->setText(tr("Dot1"));
      dot1->setEnabled(false);
      hbox->addWidget(dot1);
      dot2 = new QToolButton(this);
      dot2->setText(tr("Dot2"));
      dot2->setEnabled(false);
      hbox->addWidget(dot2);
      dot3 = new QToolButton(this);
      dot3->setText(tr("Dot3"));
      dot3->setEnabled(false);
      hbox->addWidget(dot3);
      hook = new QToolButton(this);
      hook->setText(tr("Hook"));
      hook->setEnabled(false);
      hbox->addWidget(hook);
      stem = new QToolButton(this);
      stem->setText(tr("Stem"));
      stem->setEnabled(false);
      hbox->addWidget(stem);

      layout->addLayout(hbox);

      connect(iElement, SIGNAL(enableApply()), SLOT(checkDirty()));
      connect(iNote,    SIGNAL(enableApply()), SLOT(checkDirty()));
      connect(iChord,   SIGNAL(enableApply()), SLOT(checkDirty()));
      connect(iSegment, SIGNAL(enableApply()), SLOT(checkDirty()));
      connect(dot1,     SIGNAL(clicked()),     SLOT(dot1Clicked()));
      connect(dot2,     SIGNAL(clicked()),     SLOT(dot2Clicked()));
      connect(dot3,     SIGNAL(clicked()),     SLOT(dot3Clicked()));
      connect(hook,     SIGNAL(clicked()),     SLOT(hookClicked()));
      connect(stem,     SIGNAL(clicked()),     SLOT(stemClicked()));
      }

//---------------------------------------------------------
//   checkDirty
//---------------------------------------------------------

void InspectorNote::checkDirty()
      {
      inspector->enableApply(dirty());
      }

//---------------------------------------------------------
//   setElement
//---------------------------------------------------------

void InspectorNote::setElement(Element* e)
      {
      Note* note = static_cast<Note*>(e);
      Segment* segment = note->chord()->segment();

      iElement->setElement(e);
      iNote->setElement(note);
      iChord->setElement(note->chord());
      iSegment->setElement(segment);
      dot1->setEnabled(note->dot(0));
      dot2->setEnabled(note->dot(1));
      dot3->setEnabled(note->dot(2));
      stem->setEnabled(note->chord()->stem());
      hook->setEnabled(note->chord()->hook());
      }

//---------------------------------------------------------
//   apply
//---------------------------------------------------------

void InspectorNote::apply()
      {
      Note* note    = static_cast<Note*>(inspector->element());
      Score*  score = note->score();
      score->startCmd();

      iElement->apply();
      iNote->apply();
      iChord->apply();
      iSegment->apply();

      score->setLayoutAll(true);
      score->endCmd();
      mscore->endCmd();
      }

//---------------------------------------------------------
//   dot1Clicked
//---------------------------------------------------------

void InspectorNote::dot1Clicked()
      {
      Note* note = static_cast<Note*>(inspector->element());
      if (note == 0)
            return;
      NoteDot* dot = note->dot(0);
      if (dot) {
            dot->score()->select(dot);
            inspector->setElement(dot);
            dot->score()->end();
            }
      }

//---------------------------------------------------------
//   dot2Clicked
//---------------------------------------------------------

void InspectorNote::dot2Clicked()
      {
      Note* note = static_cast<Note*>(inspector->element());
      if (note == 0)
            return;
      NoteDot* dot = note->dot(1);
      if (dot) {
            dot->score()->select(dot);
            inspector->setElement(dot);
            dot->score()->end();
            }
      }

//---------------------------------------------------------
//   dot3Clicked
//---------------------------------------------------------

void InspectorNote::dot3Clicked()
      {
      Note* note = static_cast<Note*>(inspector->element());
      if (note == 0)
            return;
      NoteDot* dot = note->dot(2);
      if (dot) {
            dot->score()->select(dot);
            inspector->setElement(dot);
            dot->score()->end();
            }
      }

//---------------------------------------------------------
//   hookClicked
//---------------------------------------------------------

void InspectorNote::hookClicked()
      {
      Note* note = static_cast<Note*>(inspector->element());
      if (note == 0)
            return;
      Hook* hook = note->chord()->hook();
      if (hook) {
            note->score()->select(hook);
            inspector->setElement(hook);
            note->score()->end();
            }
      }

//---------------------------------------------------------
//   stemClicked
//---------------------------------------------------------

void InspectorNote::stemClicked()
      {
      Note* note = static_cast<Note*>(inspector->element());
      if (note == 0)
            return;
      Stem* stem = note->chord()->stem();
      if (stem) {
            note->score()->select(stem);
            inspector->setElement(stem);
            note->score()->end();
            }
      }

//---------------------------------------------------------
//   dirty
//    return true if a property has changed
//---------------------------------------------------------

bool InspectorNote::dirty() const
      {
      return iElement->dirty()
         || iNote->dirty()
         || iChord->dirty()
         || iSegment->dirty()
         ;
      }

//---------------------------------------------------------
//   InspectorRest
//---------------------------------------------------------

InspectorRest::InspectorRest(QWidget* parent)
   : InspectorBase(parent)
      {
      iElement = new InspectorElementElement(this);
      iSegment = new InspectorSegment(this);

      layout->addWidget(iElement);
      QHBoxLayout* l = new QHBoxLayout;
      small          = new QCheckBox;
      small->setText(tr("Small"));
      l->addWidget(small);
      layout->addLayout(l);
      layout->addWidget(iSegment);
      connect(iElement, SIGNAL(enableApply()), inspector, SLOT(enableApply()));
      connect(iSegment, SIGNAL(enableApply()), inspector, SLOT(enableApply()));
      connect(small,    SIGNAL(stateChanged(int)), inspector, SLOT(enableApply()));
      }

//---------------------------------------------------------
//   setElement
//---------------------------------------------------------

void InspectorRest::setElement(Element* e)
      {
      Rest* rest = static_cast<Rest*>(e);
      Segment* segment = rest->segment();

      iElement->setElement(rest);
      iSegment->setElement(segment);
      small->setChecked(rest->small());
      }

//---------------------------------------------------------
//   apply
//---------------------------------------------------------

void InspectorRest::apply()
      {
      Rest* rest       = static_cast<Rest*>(inspector->element());
      Score* score     = rest->score();
      score->startCmd();

      iElement->apply();
      iSegment->apply();
      bool val = small->isChecked();
      if (val != rest->small())
            score->undoChangeProperty(rest, P_SMALL, val);
      score->setLayoutAll(true);
      score->endCmd();
      mscore->endCmd();
      }

//---------------------------------------------------------
//   InspectorClef
//---------------------------------------------------------

InspectorClef::InspectorClef(QWidget* parent)
   : InspectorBase(parent)
      {
      iElement = new InspectorElementElement(this);
      iSegment = new InspectorSegment(this);

      layout->addWidget(iElement);
      layout->addWidget(iSegment);
      connect(iElement, SIGNAL(enableApply()), inspector, SLOT(enableApply()));
      connect(iSegment, SIGNAL(enableApply()), inspector, SLOT(enableApply()));
      }

//---------------------------------------------------------
//   setElement
//---------------------------------------------------------

void InspectorClef::setElement(Element* e)
      {
      Clef* clef = static_cast<Clef*>(e);
      Segment* segment = clef->segment();

      iElement->setElement(clef);
      iSegment->setElement(segment);
      }

//---------------------------------------------------------
//   apply
//---------------------------------------------------------

void InspectorClef::apply()
      {
      Clef* clef   = static_cast<Clef*>(inspector->element());
      Score* score = clef->score();
      score->startCmd();

      iElement->apply();
      iSegment->apply();

      score->setLayoutAll(true);
      score->endCmd();
      mscore->endCmd();
      }

//---------------------------------------------------------
//   InspectorChord
//---------------------------------------------------------

InspectorChord::InspectorChord(QWidget* parent)
   : QWidget(parent)
      {
      setupUi(this);
      connect(small,         SIGNAL(toggled(bool)),            SLOT(smallChanged(bool)));
      connect(stemless,      SIGNAL(toggled(bool)),            SLOT(stemlessChanged(bool)));
      connect(stemDirection, SIGNAL(currentIndexChanged(int)), SLOT(stemDirectionChanged(int)));

      connect(resetSmall,    SIGNAL(clicked()),      SLOT(resetSmallClicked()));
      connect(resetStemless, SIGNAL(clicked()),      SLOT(resetStemlessClicked()));
      connect(resetStemDirection, SIGNAL(clicked()), SLOT(resetStemDirectionClicked()));
      }

//---------------------------------------------------------
//   dirty
//---------------------------------------------------------

bool InspectorChord::dirty() const
      {
      return chord->small() != small->isChecked()
         || chord->noStem() != stemless->isChecked()
         || chord->stemDirection() != (Direction)(stemDirection->currentIndex())
         ;
      }

//---------------------------------------------------------
//   setElement
//---------------------------------------------------------

void InspectorChord::setElement(Chord* c)
      {
      chord = c;

      small->blockSignals(true);
      stemless->blockSignals(true);
      stemDirection->blockSignals(true);

      small->setChecked(chord->small());
      stemless->setChecked(chord->noStem());
      stemDirection->setCurrentIndex(chord->stemDirection());

      resetSmall->setEnabled(chord->small());
      resetStemless->setEnabled(chord->noStem());
      resetStemDirection->setEnabled(stemDirection->currentIndex() != 0);

      small->blockSignals(false);
      stemless->blockSignals(false);
      stemDirection->blockSignals(false);
      }

//---------------------------------------------------------
//   smallChanged
//---------------------------------------------------------

void InspectorChord::smallChanged(bool val)
      {
      resetSmall->setEnabled(val);
      emit enableApply();
      }

//---------------------------------------------------------
//   stemlessChanged
//---------------------------------------------------------

void InspectorChord::stemlessChanged(bool val)
      {
      resetStemless->setEnabled(val);
      emit enableApply();
      }

//---------------------------------------------------------
//   stemDirectionChanged
//---------------------------------------------------------

void InspectorChord::stemDirectionChanged(int idx)
      {
      resetStemDirection->setEnabled(idx != 0);
      emit enableApply();
      }

//---------------------------------------------------------
//   resetSmall
//---------------------------------------------------------

void InspectorChord::resetSmallClicked()
      {
      small->setChecked(false);
      }

//---------------------------------------------------------
//   resetStemless
//---------------------------------------------------------

void InspectorChord::resetStemlessClicked()
      {
      stemless->setChecked(false);
      }

//---------------------------------------------------------
//   resetStemDirection
//---------------------------------------------------------

void InspectorChord::resetStemDirectionClicked()
      {
      stemDirection->setCurrentIndex(0);
      }

//---------------------------------------------------------
//   apply
//---------------------------------------------------------

void InspectorChord::apply()
      {
      Score* score = chord->score();
      if (small->isChecked() != chord->small())
            score->undoChangeProperty(chord, P_SMALL, small->isChecked());
      if (stemless->isChecked() != chord->noStem())
            score->undoChangeProperty(chord, P_NO_STEM, stemless->isChecked());
      Direction d = Direction(stemDirection->currentIndex());
      if (d != chord->stemDirection())
            score->undoChangeProperty(chord, P_STEM_DIRECTION, d);
      }


