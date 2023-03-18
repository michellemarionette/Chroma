#include "Headers.h"


FrequencyEstimator::FrequencyEstimator() {}
FrequencyEstimator::~FrequencyEstimator() {}

chroma::NoteInfo& FrequencyEstimator::getLastNote()
{
    updateNote();
    return m_note;
}

void FrequencyEstimator::prepareToPlay(double sampleRate, float lowestFrequency, float highestFrequency)
{
    cycfi::q::frequency lowest_freq(lowestFrequency);
    cycfi::q::frequency highest_freq(highestFrequency);
    cycfi::q::decibel hysteresis(0);
    
    m_bacf = std::make_shared<cycfi::q::pitch_detector>(lowest_freq, highest_freq, sampleRate, hysteresis);
}

void FrequencyEstimator::processBlock(juce::AudioBuffer<float>& buffer)
{
    if (buffer.getRMSLevel(0, 0, buffer.getNumSamples()) < Variables::rmsThreshold)
    {
        m_bacf->reset();
        updateFrequency();
        return;
    }
    
    auto* channelData = buffer.getWritePointer(0);
    
    for (int i = 0; i < buffer.getNumSamples(); ++i)
    {
        (*m_bacf)(channelData[i]);
        updateFrequency();
    }
}

float FrequencyEstimator::getFrequency()
{
    return m_frequency.load();
}

void FrequencyEstimator::updateFrequency()
{
    m_frequency = m_bacf->get_frequency();
    
    if (m_frequency == 0.f)
    {
        m_frequency = -1;
    }
}

void FrequencyEstimator::updateNote()
{
    auto frequency = getFrequency();

    if (m_note.frequency != frequency)
    {
        m_note.frequency = frequency;
        m_note.note = chroma::Midi::frequencyToMidi(frequency);
        m_note.cents = chroma::Midi::centsFromNearestNote(frequency); 
    }

    else if (frequency == -1)
    {
        m_note.frequency = -1;
        m_note.note = -1;
        m_note.cents = -1;
    }
}

