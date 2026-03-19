/*
 * Auto-generated from: 17_Ada_SPARK\05_Case_Studies\04_F_35_Lightning_II.md
 * Line: 93
 * Language: c
 * Block ID: a6a8f77b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// flight_control.h
class FlightControl {
public:
    void updateControlLaw();
    void setMode(ControlMode mode);

private:
    ControlState state;
    PIDController pitchPID;
    PIDController rollPID;
};

// flight_control.cpp
void FlightControl::updateControlLaw() {
    // 潜在问题：无边界检查
    float pitchCmd = pitchPID.calculate(currentPitch, desiredPitch);
    float rollCmd = rollPID.calculate(currentRoll, desiredRoll);

    // 潜在问题：除零风险
    float gain = 1.0f / (airspeed * 0.01f);

    // 潜在问题：数组越界
    controlSurfaces[surfaceIndex] = pitchCmd * gain;
}
