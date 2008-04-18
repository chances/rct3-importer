#include "validator_keyframes.h"

#include <boost/format.hpp>

#include "matrix.h"
#include "xmlhelper.h"

wxString dlgKeyframes::GetMethodString(int type, int method) {
    if (type) {
        switch (method) {
            case Method_SimpleLinear:
                return _("Simple (constant omega)");
            case Method_SimpleAcceleration:
                return _("Simple (constant alpha)");
            case Method_SimpleJerk:
                return _("Simple (constant jerk)");
            case Method_AccelDeccel:
                return _("Accelerate - decelerate");
            case Method_SineBob:
                return _("Sine bob");
            case Method_Dampened:
                return _("Dampened oscillation");
        }
    } else {
        switch (method) {
            case Method_SimpleLinear:
                return _("Simple (constant v)");
            case Method_SimpleAcceleration:
                return _("Simple (constant a)");
            case Method_SimpleJerk:
                return _("Simple (constant jerk)");
            case Method_AccelDeccel:
                return _("Accelerate - decelerate");
            case Method_SineBob:
                return _("Sine bob");
            case Method_Dampened:
                return _("Dampened oscillation");
        }
    }
    return "UNDEFINED";
}

dlgKeyframes::dlgKeyframes( wxWindow* parent, wxStyledTextCtrl* stc ): m_init(false), m_stc(stc), rcdlgKeyframes( parent ) {
//wxMessageBox("Constructor");
    m_textEVal[0] = m_textEValX;
    m_textEVal[1] = m_textEValY;
    m_textEVal[2] = m_textEValZ;
    m_textEVel[0] = m_textEVelX;
    m_textEVel[1] = m_textEVelY;
    m_textEVel[2] = m_textEVelZ;
    m_textPer[0] = m_textPerX;
    m_textPer[1] = m_textPerY;
    m_textPer[2] = m_textPerZ;
    m_textPerOff[0] = m_textPerOffX;
    m_textPerOff[1] = m_textPerOffY;
    m_textPerOff[2] = m_textPerOffZ;
    m_textSVal[0] = m_textSValX;
    m_textSVal[1] = m_textSValY;
    m_textSVal[2] = m_textSValZ;
    m_textSVel[0] = m_textSVelX;
    m_textSVel[1] = m_textSVelY;
    m_textSVel[2] = m_textSVelZ;
    m_textVal[0] = m_textValX;
    m_textVal[1] = m_textValY;
    m_textVal[2] = m_textValZ;
    m_choiceMethod[0] = m_choiceMethodX;
    m_choiceMethod[1] = m_choiceMethodY;
    m_choiceMethod[2] = m_choiceMethodZ;

    m_init = true;
    DoTypeChange();
}

/** @brief OnClose
  *
  * @todo: document this function
  */
void dlgKeyframes::OnClose(wxCommandEvent& event) {
    EndModal(0);
}

/** @brief OnInsertClose
  *
  * @todo: document this function
  */
void dlgKeyframes::OnInsertClose(wxCommandEvent& event) {
    OnInsert(event);
    OnClose(event);
}

/** @brief OnInsert
  *
  * @todo: document this function
  */
void dlgKeyframes::OnInsert(wxCommandEvent& event) {
    int currentLine = m_stc->GetCurrentLine();
    int lineInd = m_stc->GetLineIndentation(currentLine);
    wxString indentstr;
    if (m_stc->GetUseTabs())
        indentstr = wxString('\t', lineInd / m_stc->GetTabWidth());
    else
        indentstr = wxString('\t', lineInd);
    int inspos = m_stc->PositionFromLine(currentLine);

    int type = m_choiceType->GetSelection();
    wxString fr = type?"rotate":"translate";

    wxString res;
    res += wxString::Format("%s\n", indentstr.c_str());
    wxString comment = m_textComment->GetValue();
    if (!comment.IsEmpty())
        res += wxString::Format("%s<!-- %s -->\n", indentstr.c_str(), comment);
    res += wxString::Format("%s<!-- X: %s -->\n", indentstr.c_str(), GetMethodString(type, m_choiceMethodX->GetSelection()));
    res += wxString::Format("%s<!-- Y: %s -->\n", indentstr.c_str(), GetMethodString(type, m_choiceMethodY->GetSelection()));
    res += wxString::Format("%s<!-- Z: %s -->\n", indentstr.c_str(), GetMethodString(type, m_choiceMethodZ->GetSelection()));
    res += wxString::Format("%s<!-- Frames: %lu -->\n", indentstr.c_str(), m_frames);
    res += wxString::Format("%s<!-- Value: %s / %s / %s -->\n", indentstr.c_str(), m_textSValX->GetValue(),  m_textSValY->GetValue(),  m_textSValZ->GetValue());
    res += wxString::Format("%s<!-- Speed: %s / %s / %s -->\n", indentstr.c_str(), m_textSVelX->GetValue(),  m_textSVelY->GetValue(),  m_textSVelZ->GetValue());

    for (int i = m_cbWriteFirst->IsChecked()?0:1; i < m_frames; ++i) {
        res += boost::str(boost::format("%s<%s time=\"%.6f\" x=\"%.6f\" y=\"%.6f\" z=\"%.6f\"/>\n") % indentstr.c_str() % fr.c_str() %
            m_times[i] % m_axes[0][i] % m_axes[1][i] % m_axes[2][i]);
    }
    res += wxString::Format("%s<!-- Value: %s / %s / %s -->\n", indentstr.c_str(), m_textEValX->GetValue(),  m_textEValY->GetValue(),  m_textEValZ->GetValue());
    res += wxString::Format("%s<!-- Speed: %s / %s / %s -->\n", indentstr.c_str(), m_textEVelX->GetValue(),  m_textEVelY->GetValue(),  m_textEVelZ->GetValue());

    m_stc->InsertText(inspos, res);
    wxString val[3];
    wxString vel[3];
    m_cbWriteFirst->SetValue(false);
    for (int i = 0; i < 3; ++i) {
        val[i] = m_textEVal[i]->GetValue();
        vel[i] = m_textEVel[i]->GetValue();
    }
    float etime = 0.0;
    parseFloatC(m_textEndTime->GetValue(), etime);

    Init();

    for (int i = 0; i < 3; ++i) {
        m_textSVal[i]->ChangeValue(val[i]);
        m_textSVel[i]->ChangeValue(vel[i]);
        m_choiceMethod[i]->SetSelection(Method_SimpleAcceleration);
    }
    m_textStartTime->ChangeValue(boost::str(boost::format("%.6f") % etime));
    m_textEndTime->SetValue(boost::str(boost::format("%.6f") % (etime+2)));

}

/** @brief OnMethodChange
  *
  * @todo: document this function
  */
void dlgKeyframes::OnMethodChange(wxCommandEvent& event) {
    if (!m_init)
        return;

    if (event.GetEventObject() == m_choiceMethodX) {
        DoCalculation(0);
    } else if (event.GetEventObject() == m_choiceMethodY) {
        DoCalculation(1);
    } else {
        DoCalculation(2);
    }
}

/** @brief OnValueXChange
  *
  * @todo: document this function
  */
void dlgKeyframes::OnValueXChange(wxCommandEvent& event) {
//wxMessageBox("OnValueXChange");
    if (!m_init)
        return;
    DoCalculation(0);
}

/** @brief OnValueYChange
  *
  * @todo: document this function
  */
void dlgKeyframes::OnValueYChange(wxCommandEvent& event) {
    if (!m_init)
        return;
    DoCalculation(1);
}

/** @brief OnValueZChange
  *
  * @todo: document this function
  */
void dlgKeyframes::OnValueZChange(wxCommandEvent& event) {
    if (!m_init)
        return;
    DoCalculation(2);
}

/** @brief OnTypeChange
  *
  * @todo: document this function
  */
void dlgKeyframes::OnTypeChange(wxCommandEvent& event) {
    if (!m_init)
        return;

    DoTypeChange();
}

/** @brief DoTypeChange
  *
  * @todo: document this function
  */
void dlgKeyframes::DoTypeChange() {
//wxMessageBox("DoTypeChange");
    int i = m_choiceType->GetSelection();
    Init();
    if (i) {
        // Rotation
        // Add Interpolation types
        for(int x = 0; x < 3; ++x) {
            m_choiceMethod[x]->Clear();
            for (int t = 0; t < Method_MAX; ++t)
                m_choiceMethod[x]->Append(GetMethodString(i, t));
        }

        DoGridChange();

        for(int x = 0; x < 3; ++x) {
            m_choiceMethod[x]->SetSelection(Method_SimpleLinear);
            DoCalculation(x);
        }
    } else {
        // Translation

        // Add Interpolation types
        for(int x = 0; x < 3; ++x) {
            m_choiceMethod[x]->Clear();
            for (int t = 0; t < Method_MAX; ++t)
                m_choiceMethod[x]->Append(GetMethodString(i, t));
        }

        DoGridChange();

        for(int x = 0; x < 3; ++x) {
            m_choiceMethod[x]->SetSelection(Method_SimpleLinear);
            DoCalculation(x);
        }
    }
}

/** @brief Init
  *
  * @todo: document this function
  */
void dlgKeyframes::Init() {
//wxMessageBox("Init");
    m_textStartTime->ChangeValue("0.0");
    m_textEndTime->ChangeValue("2.0");
    if (m_choiceFrames->GetSelection())
        m_textFrameNumber->ChangeValue("1.0");
    else
        m_textFrameNumber->ChangeValue("2");
    for(int x = 0; x < 3; ++x) {
        m_textEVal[x]->ChangeValue("0.0");
        m_textEVel[x]->ChangeValue("0.0");
        m_textPer[x]->ChangeValue("0.0");
        m_textPerOff[x]->ChangeValue("0.0");
        m_textVal[x]->ChangeValue("0.0");
        m_textSVal[x]->ChangeValue("0.0");
        m_textSVel[x]->ChangeValue("0.0");
    }
}

/** @brief OnGridChange
  *
  * @todo: document this function
  */
void dlgKeyframes::OnGridChange(wxCommandEvent& event) {
    DoGridChange();
}

/** @brief DoGridChange
  *
  * @todo: document this function
  */
void dlgKeyframes::DoGridChange() {
//wxMessageBox("DoGridChange");
    float start_time = 0.0;
    if (!parseFloatC(m_textStartTime->GetValue(), start_time))
        return;
    if (start_time < 0)
        return;
    float end_time = 0.0;
    if (!parseFloatC(m_textEndTime->GetValue(), end_time))
        return;
    if (end_time < 0)
        return;
    if (end_time <= start_time)
        return;
    float frame_count = 0.0;
    if (!parseFloatC(m_textFrameNumber->GetValue(), frame_count))
        return;

    if (m_choiceFrames->GetSelection())
        // FPS
        frame_count = roundf((end_time-start_time)*frame_count)+1;
    else
        frame_count = floorf(frame_count);

    if (static_cast<unsigned long>(frame_count) < 2)
        return;

    m_frames = frame_count;
    m_startTime = start_time;
    m_endTime = end_time;

    m_gridVals->DeleteRows(0, m_gridVals->GetNumberRows());
    m_times.clear();
    m_gridVals->AppendRows(frame_count);
    for (int i = 0; i < frame_count; ++i) {
        float t = start_time + (static_cast<float>(i)*(end_time-start_time)/static_cast<float>(frame_count-1));
        m_times.push_back(t);
        m_gridVals->SetRowLabelValue(i, boost::str(boost::format("%.2f") % t));
    }

    DoCalculation(0);
    DoCalculation(1);
    DoCalculation(2);
}

/** @brief DoCalculation
  *
  * @todo: document this function
  */
void dlgKeyframes::DoCalculationT(int axis) {
    int i = m_choiceType->GetSelection();
    if (i)
        DoCalculationR(axis);
    else
        DoCalculationT(axis);
}

inline void normalizeAngle(float& a) {
    a = fmodf(a, 360.0);
    if (a<0)
        a += 360.0;
}

inline float calcRotDistance(float from, float to, float cycles) {
    float dist;

    if (cycles < 0) {
        if (from < to)
            dist = -((360.0 - to) + from);
        else
            dist = to - from;
    } else {
        if (from > to)
            dist = (360.0 - from) + to;
        else
            dist = to - from;
    }

    return dist + floorf(cycles) * 360.0;
}

/** @brief DoCalculationT
  *
  * @todo: document this function
  */
void dlgKeyframes::DoCalculation(int axis) {
//wxMessageBox("DoCalculation");
    int i = m_choiceMethod[axis]->GetSelection();
    if (i < 0)
        return;

    bool rotate = m_choiceType->GetSelection();

    switch (i) {
        case Method_SimpleLinear: {
            m_textEVal[axis]->Enable(true);
            m_textEVel[axis]->Enable(false);
            m_textPer[axis]->Enable(false);
            m_textSVal[axis]->Enable(true);
            m_textSVel[axis]->Enable(false);
            m_textVal[axis]->Enable(false);
            m_textPerOff[axis]->Enable(false);

            float valmin = 0.0;
            if (!parseFloatC(m_textSVal[axis]->GetValue(), valmin))
                return;
            float valmax = 0.0;
            if (!parseFloatC(m_textEVal[axis]->GetValue(), valmax))
                return;

            float len = valmax-valmin;
            if (rotate) {
                normalizeAngle(valmin);
                normalizeAngle(valmax);
            }

            m_axes[axis].clear();
            for (int i = 0; i < m_frames; ++i) {
                float t = valmin + (static_cast<float>(i)*(len)/static_cast<float>(m_frames-1));
                if (rotate) {
                    normalizeAngle(t);
                    m_axes[axis].push_back(Deg2Rad(t));
                } else {
                    m_axes[axis].push_back(t);
                }
                m_gridVals->GetTable()->SetValue(i, axis, boost::str(boost::format("%.6f") % t));
            }
            m_gridVals->ForceRefresh();

            m_textVal[axis]->ChangeValue(boost::str(boost::format("%.6f") % ((len)/2)));
            m_textSVel[axis]->ChangeValue(boost::str(boost::format("%.6f") % ((len)/(m_endTime-m_startTime))));
            m_textEVel[axis]->ChangeValue(boost::str(boost::format("%.6f") % ((len)/(m_endTime-m_startTime))));
            break;
        }
        case Method_SimpleAcceleration: {
            m_textEVal[axis]->Enable(true);
            m_textEVel[axis]->Enable(false);
            m_textPer[axis]->Enable(false);
            m_textSVal[axis]->Enable(true);
            m_textSVel[axis]->Enable(true);
            m_textVal[axis]->Enable(false);
            m_textPerOff[axis]->Enable(false);

            float valmin = 0.0;
            if (!parseFloatC(m_textSVal[axis]->GetValue(), valmin))
                return;
            float velmin = 0.0;
            if (!parseFloatC(m_textSVel[axis]->GetValue(), velmin))
                return;
            float valmax = 0.0;
            if (!parseFloatC(m_textEVal[axis]->GetValue(), valmax))
                return;

            float tim = m_endTime-m_startTime;
            float len = valmax-valmin;
            float acc = 2*(len - (tim*velmin))/(tim*tim);

            if (rotate) {
                normalizeAngle(valmin);
                normalizeAngle(valmax);
            }

            m_axes[axis].clear();
            for (int i = 0; i < m_frames; ++i) {
                float t = static_cast<float>(i)*tim/static_cast<float>(m_frames-1);
                float s = (velmin * t) + (0.5 * acc * t * t);
                s += valmin;
                if (rotate) {
                    normalizeAngle(s);
                    m_axes[axis].push_back(Deg2Rad(s));
                } else {
                    m_axes[axis].push_back(s);
                }
                m_gridVals->GetTable()->SetValue(i, axis, boost::str(boost::format("%.6f") % s));
            }
            m_gridVals->ForceRefresh();

            m_textVal[axis]->ChangeValue(boost::str(boost::format("%.6f") % (velmin + (acc*tim/2))));
            m_textPerOff[axis]->ChangeValue(boost::str(boost::format("%.6f") % (acc)));
            m_textEVel[axis]->ChangeValue(boost::str(boost::format("%.6f") % (velmin + (acc*tim))));
            break;
        }
        case Method_SimpleJerk: {
            m_textEVal[axis]->Enable(true);
            m_textEVel[axis]->Enable(true);
            m_textPer[axis]->Enable(false);
            m_textSVal[axis]->Enable(true);
            m_textSVel[axis]->Enable(true);
            m_textVal[axis]->Enable(false);
            m_textPerOff[axis]->Enable(false);

            float valmin = 0.0;
            if (!parseFloatC(m_textSVal[axis]->GetValue(), valmin))
                return;
            float velmin = 0.0;
            if (!parseFloatC(m_textSVel[axis]->GetValue(), velmin))
                return;
            float velmax = 0.0;
            if (!parseFloatC(m_textEVel[axis]->GetValue(), velmax))
                return;
            float valmax = 0.0;
            if (!parseFloatC(m_textEVal[axis]->GetValue(), valmax))
                return;

            float tim = m_endTime-m_startTime;
            float len = valmax-valmin;
            float jerk = 6 * ((tim * (velmin + velmax)) - 2*len)/(tim*tim*tim);
            float a0 = (2.0*(((len - (velmin*tim))/(tim*tim)) - (jerk*tim/6.0)));

            if (rotate) {
                normalizeAngle(valmin);
                normalizeAngle(valmax);
            }

            m_axes[axis].clear();
            for (int i = 0; i < m_frames; ++i) {
                float t = static_cast<float>(i)*tim/static_cast<float>(m_frames-1);
                float s = (velmin * t) + (0.5 * a0 * t * t) + ((1.0/6.0) * jerk * t * t * t);
                s += valmin;
                if (rotate) {
                    normalizeAngle(s);
                    m_axes[axis].push_back(Deg2Rad(s));
                } else {
                    m_axes[axis].push_back(s);
                }
                m_gridVals->GetTable()->SetValue(i, axis, boost::str(boost::format("%.6f") % s));
            }
            m_gridVals->ForceRefresh();

            m_textPer[axis]->ChangeValue(boost::str(boost::format("%.6f") % a0));
            m_textPerOff[axis]->ChangeValue(boost::str(boost::format("%.6f") % jerk));
            m_textVal[axis]->ChangeValue(boost::str(boost::format("%.6f") % (a0 + jerk*tim)));

            break;
        }
        case Method_AccelDeccel: {
            m_textEVal[axis]->Enable(true);
            m_textEVel[axis]->Enable(true);
            m_textPer[axis]->Enable(false);
            m_textSVal[axis]->Enable(true);
            m_textSVel[axis]->Enable(true);
            m_textVal[axis]->Enable(false);
            m_textPerOff[axis]->Enable(false);

            float valmin = 0.0;
            if (!parseFloatC(m_textSVal[axis]->GetValue(), valmin))
                return;
            float velmin = 0.0;
            if (!parseFloatC(m_textSVel[axis]->GetValue(), velmin))
                return;
            float velmax = 0.0;
            if (!parseFloatC(m_textEVel[axis]->GetValue(), velmax))
                return;
            float valmax = 0.0;
            if (!parseFloatC(m_textEVal[axis]->GetValue(), valmax))
                return;

            float tim = m_endTime-m_startTime;
            float len = valmax-valmin;
            float acc1 = (((2.0*len) - (1.414213562373095*pow(((2.0*((len*len) - (tim*len*(velmin + velmax)))) + ((tim*tim)*((velmin*velmin) + (velmax*velmax)))), (1.0/2.0))) - (tim*(velmax + velmin)))/(tim*tim));
            float acc2 = (((2.0*len) - (1.414213562373095*pow(((2.0*((len*len) - (tim*len*(velmin + velmax)))) + ((tim*tim)*((velmin*velmin) + (velmax*velmax)))), (1.0/2.0))*(-1)) - (tim*(velmax + velmin)))/(tim*tim));
            float acc;
            float t1;

            if (rotate) {
                normalizeAngle(valmin);
                normalizeAngle(valmax);
            }

            if (fabs(acc1) < 0.000001) {
                if (fabs(acc2) < 0.000001) {
                    m_textVal[axis]->ChangeValue("Collapsed!");
                    return;
                }
                acc = acc2;
                t1 = (-1.0*(((velmin - velmax + (tim*acc2))/(2.0*acc2)) - tim));
            } else {
                if (fabs(acc2) < 0.000001) {
                    acc = acc1;
                    t1 = (-1.0*(((velmin - velmax + (tim*acc1))/(2.0*acc1)) - tim));
                } else {
                    // Two solutions, we need to decide by the time
                    float t11 = (-1.0*(((velmin - velmax + (tim*acc1))/(2.0*acc1)) - tim));
                    float t12 = (-1.0*(((velmin - velmax + (tim*acc2))/(2.0*acc2)) - tim));
                    if ((t11 < 0) || (t11 > tim)) {
                        acc = acc2;
                        t1 = t12;
                    } else {
                        acc = acc1;
                        t1 = t11;
                    }
                }
            }
            float s1 = (velmin * t1) + (0.5 * acc * t1 * t1);
            float vm = ((t1*acc) + velmin);

            m_axes[axis].clear();
            for (int i = 0; i < m_frames; ++i) {
                float t = static_cast<float>(i)*tim/static_cast<float>(m_frames-1);
                float s;
                if (t < t1) {
                    s = (velmin * t) + (0.5 * acc * t * t);
                } else {
                    s = s1 + (vm * (t-t1)) - (0.5 * acc * (t-t1) * (t-t1));
                }
                s += valmin;
                if (rotate) {
                    normalizeAngle(s);
                    m_axes[axis].push_back(Deg2Rad(s));
                } else {
                    m_axes[axis].push_back(s);
                }
                m_gridVals->GetTable()->SetValue(i, axis, boost::str(boost::format("%.6f") % s));
            }
            m_gridVals->ForceRefresh();

            m_textVal[axis]->ChangeValue(boost::str(boost::format("%.6f") % vm));

            break;
        }
        case Method_SineBob: {
            m_textEVal[axis]->Enable(false);
            m_textEVel[axis]->Enable(false);
            m_textPer[axis]->Enable(true);
            m_textSVal[axis]->Enable(true);
            m_textSVel[axis]->Enable(false);
            m_textVal[axis]->Enable(true);
            m_textPerOff[axis]->Enable(true);

            float valmin = 0.0;
            if (!parseFloatC(m_textSVal[axis]->GetValue(), valmin))
                return;
            float val = 0.0;
            if (!parseFloatC(m_textVal[axis]->GetValue(), val))
                return;
            if (fabs(val) < 0.000001)
                return;
            float per = 0.0;
            if (!parseFloatC(m_textPer[axis]->GetValue(), per))
                return;
            if (per <= 0.0)
                return;
            float peroff = 0.0;
            if (!parseFloatC(m_textPerOff[axis]->GetValue(), peroff))
                return;

            float tim = m_endTime-m_startTime;
            // y = a sin(bx + c)
            // a = val
            float b = 2.0*M_PI*per/tim;
            float c = -2.0*M_PI*peroff;

            m_axes[axis].clear();
            for (int i = 0; i < m_frames; ++i) {
                float t = static_cast<float>(i)*tim/static_cast<float>(m_frames-1);
                float s = val * sin((b*t)+c);
                s += valmin;
                if (rotate) {
                    normalizeAngle(s);
                    m_axes[axis].push_back(Deg2Rad(s));
                } else {
                    m_axes[axis].push_back(s);
                }
                m_gridVals->GetTable()->SetValue(i, axis, boost::str(boost::format("%.6f") % s));
            }
            m_gridVals->ForceRefresh();

            m_textEVal[axis]->ChangeValue(boost::str(boost::format("%.6f") % ((val * sin((b*tim)+c))+valmin)));
            m_textSVel[axis]->ChangeValue(boost::str(boost::format("%.6f") % (val*b*cos(c))));
            m_textEVel[axis]->ChangeValue(boost::str(boost::format("%.6f") % (val*b*cos(c+b*tim))));
            break;
        }
        case Method_Dampened: {
            m_textEVal[axis]->Enable(false);
            m_textEVel[axis]->Enable(false);
            m_textPer[axis]->Enable(true);
            m_textSVal[axis]->Enable(true);
            m_textSVel[axis]->Enable(true);
            m_textVal[axis]->Enable(true);
            m_textPerOff[axis]->Enable(false);

            float valmin = 0.0;
            if (!parseFloatC(m_textSVal[axis]->GetValue(), valmin))
                return;
            float val = 0.0;
            if (!parseFloatC(m_textVal[axis]->GetValue(), val))
                return;
            float per = 0.0;
            if (!parseFloatC(m_textPer[axis]->GetValue(), per))
                return;
            if (per <= 0.0)
                return;
            float velmin = 0.0;
            if (!parseFloatC(m_textSVel[axis]->GetValue(), velmin))
                return;

            float tim = m_endTime-m_startTime;
            float y0 = val;
            float f = - velmin / (2.0 * M_PI * y0);

            if (rotate) {
                normalizeAngle(valmin);
            }

            m_axes[axis].clear();
            for (int i = 0; i < m_frames; ++i) {
                float t = static_cast<float>(i)*tim/static_cast<float>(m_frames-1);
                float s = y0 * exp(-t/per) * sin(2.0*M_PI*f*t);
                s = valmin - s;
                if (rotate) {
                    normalizeAngle(s);
                    m_axes[axis].push_back(Deg2Rad(s));
                } else {
                    m_axes[axis].push_back(s);
                }
                m_gridVals->GetTable()->SetValue(i, axis, boost::str(boost::format("%.6f") % s));
            }
            m_gridVals->ForceRefresh();

            float vmax = ((2 * f * M_PI * per * y0 * cos(2 * f * M_PI * tim)) - (y0 * sin(2 * f * M_PI * tim))) / (per * exp(tim/per));
            m_textEVal[axis]->ChangeValue(boost::str(boost::format("%.6f") % (valmin-(y0 * exp(-tim/per) * sin(2.0*M_PI*f*tim)))));
            m_textEVel[axis]->ChangeValue(boost::str(boost::format("%.6f") % (vmax)));
            break;
        }
    }
}

/** @brief DoCalculationR
  *
  * @todo: document this function
  */
void dlgKeyframes::DoCalculationR(int axis) {
    int i = m_choiceMethod[axis]->GetSelection();
    if (i < 0)
        return;

    switch (i) {
        case Method_R_SimpleLinear: {
            m_textEVal[axis]->Enable(true);
            m_textEVel[axis]->Enable(false);
            m_textPer[axis]->Enable(true);
            m_textSVal[axis]->Enable(true);
            m_textSVel[axis]->Enable(false);
            m_textVal[axis]->Enable(false);
            m_textPerOff[axis]->Enable(false);

            float valmin = 0.0;
            if (!parseFloatC(m_textSVal[axis]->GetValue(), valmin))
                return;
            float valmax = 0.0;
            if (!parseFloatC(m_textEVal[axis]->GetValue(), valmax))
                return;
            float per = 0.0;
            if (!parseFloatC(m_textPer[axis]->GetValue(), per))
                return;

            // normalize input
            normalizeAngle(valmin);
            normalizeAngle(valmax);

            float dist = calcRotDistance(valmin, valmax, per);

            m_axes[axis].clear();
            for (int i = 0; i < m_frames; ++i) {
                float t = valmin + (static_cast<float>(i)*(dist)/static_cast<float>(m_frames-1));
                normalizeAngle(t);
                m_gridVals->GetTable()->SetValue(i, axis, boost::str(boost::format("%.3f") % t));
                m_axes[axis].push_back(Deg2Rad(t));
            }
            m_gridVals->ForceRefresh();

            m_textVal[axis]->ChangeValue(boost::str(boost::format("%.3f") % ((dist)/2)));
            m_textSVel[axis]->ChangeValue(boost::str(boost::format("%.3f") % ((dist)/(m_endTime-m_startTime))));
            m_textEVel[axis]->ChangeValue(boost::str(boost::format("%.3f") % ((dist)/(m_endTime-m_startTime))));
            break;
        }
        case Method_R_SimpleAcceleration: {
            m_textEVal[axis]->Enable(true);
            m_textEVel[axis]->Enable(false);
            m_textPer[axis]->Enable(true);
            m_textSVal[axis]->Enable(true);
            m_textSVel[axis]->Enable(true);
            m_textVal[axis]->Enable(false);
            m_textPerOff[axis]->Enable(false);

            float valmin = 0.0;
            if (!parseFloatC(m_textSVal[axis]->GetValue(), valmin))
                return;
            float velmin = 0.0;
            if (!parseFloatC(m_textSVel[axis]->GetValue(), velmin))
                return;
            float valmax = 0.0;
            if (!parseFloatC(m_textEVal[axis]->GetValue(), valmax))
                return;
            float per = 0.0;
            if (!parseFloatC(m_textPer[axis]->GetValue(), per))
                return;

            // normalize input
            normalizeAngle(valmin);
            normalizeAngle(valmax);

            float tim = m_endTime-m_startTime;
            float len = calcRotDistance(valmin, valmax, per);
            float acc = 2*(len - (tim*velmin))/(tim*tim);

            m_axes[axis].clear();
            for (int i = 0; i < m_frames; ++i) {
                float t = static_cast<float>(i)*tim/static_cast<float>(m_frames-1);
                float s = (velmin * t) + (0.5 * acc * t * t);
                s += valmin;
                normalizeAngle(s);
                m_axes[axis].push_back(Deg2Rad(s));
                m_gridVals->GetTable()->SetValue(i, axis, boost::str(boost::format("%.3f") % s));
            }
            m_gridVals->ForceRefresh();

            m_textVal[axis]->ChangeValue(boost::str(boost::format("%.3f") % (velmin + (acc*tim/2))));
            m_textEVel[axis]->ChangeValue(boost::str(boost::format("%.3f") % (velmin + (acc*tim))));
            break;
        }
    }
}



