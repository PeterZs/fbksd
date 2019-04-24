/*
 * Copyright (c) 2019 Jonas Deyson
 *
 * This software is released under the MIT License.
 *
 * You should have received a copy of the MIT License
 * along with this program. If not, see <https://opensource.org/licenses/MIT>
 */

#ifndef SAMPLESPIPE_H
#define SAMPLESPIPE_H

#include "fbksd/renderer/samples.h"
#include "fbksd/core/SampleLayout.h"
#include "fbksd/core/definitions.h"

namespace fbksd
{

// This is need for the next classes due to the static members
#define EXPORT_LIB __attribute__((visibility("default")))


/**
 * \brief A SampleBuffer stores data for one sample and is used to get/send data to a SamplesPipe.
 *
 * When the client sets the sample layout, it specifies two types of values:
 * - Random parameters
 * - Features
 *
 * Random parameters are values typically generated by a random sampler generator.
 * These values can be specified as INPUT (provided by the client) or OUTPUT (provided by the renderer).
 *
 * Features are the values computed by a rendering algorithm (e.g. color, depth, etc.)
 */
class EXPORT_LIB SampleBuffer
{
public:
    SampleBuffer();

    /**
     * \brief Conditionally writes a random parameters value to the buffer.
     *
     * This method only writes the value to the buffer if the random parameter was specified as
     * OUTPUT by the client, otherwise this call has no effect.
     *
     * @return The resulting value in the buffer.
     */
    float set(RandomParameter i, float v);

    /**
     * @brief Returns a random parameter value.
     */
    float get(RandomParameter i);

    /**
     * \brief Write a feature value to the buffer
     *
     * @return The value v.
     */
    float set(Feature f, float v);

    /**
    * \brief Write a numbered feature value to the buffer.
    *
    * Numbered features are features that have one instance for each intersection point
    * in a path from the camera.
    * For example, world position x can be for the first intersection point (number 0: WORLD_X) or
    * the second (number 1: WORLD_X_1).
    *
    * @return The value v.
    */
    float set(Feature f, int number, float v);

    /**
     * @brief Return a feature value.
     */
    float get(Feature f);

private:
    friend class SamplesPipe;

    static void setLayout(const SampleLayout& layout);

    // ioMask is only for random parameters, since features are always OUTPUT.
    static std::array<bool, NUM_RANDOM_PARAMETERS> m_ioMask;
    std::array<float, NUM_RANDOM_PARAMETERS> m_paramentersBuffer;
    std::array<float, NUM_FEATURES> m_featuresBuffer;
};


/**
 * \brief The SamplesPipe class is the main way of transferring samples between client and server.
 *
 * You can thing of this as a big memory region where samples are saved by the renderer
 * and read by the client.
 *
 * During the rendering process, a rendering thread typically:
 * - instantiates a SamplesPipe
 * - for each pixel to be rendered
 *   - sets the position of the pipe using one of the seek methods
 *   - calls getBuffer() to get a SampleBuffer for the current sample
 *   - renders the sample and save the data into the SampleBuffer
 *   - inserts the SampleBuffer into the pipe using the operator<<()
 *
 * Several rendering threads can work in parallel, each one having their own pipe.
 * In this case, they have to make sure that a pipe position is not written by
 * different threads.
 */
class EXPORT_LIB SamplesPipe
{
public:
    /**
     * @brief Acquires a pipe for reading/writing samples.
     *
     * The constructor tries to acquires exclusive hold of a buffer tile. If no buffer tile is available,
     * the constructor blocks until one is available.
     *
     * @param begin, end
     * Define the window in the image where the pipe will cover.
     * @param numSamples
     * Number of samples that will be inserted in this pipe. The total number of samples
     */
    SamplesPipe(const Point2l& begin, const Point2l& end, int64_t numSamples);

    SamplesPipe(SamplesPipe&& pipe);

    /**
     * @brief Releases the pipe, signaling that the it's ready to be consumed by the client.
     */
    ~SamplesPipe();

    /**
     * @brief Sets the pipe position using (x, y) pixel position.
     *
     * This is a more convenient whey of setting the pipe position
     * using a (x, y) pixel position.
     *
     * @param x, y
     * Pixel position.
     */
    void seek(int x, int y);

    /**
     * @brief Returns the current pipe position.
     */
    size_t getPosition() const;

    /**
     * @brief Returns the number of samples inserted so far.
     */
    size_t getNumSamples() const;

    /**
     * @brief Returns a SampleBuffer for the current pipe position.
     *
     * If the client specified a sample layout with input random parameters,
     * the returned sample buffer will contain the written by the client.
     */
    SampleBuffer getBuffer();

    /**
     * @brief Inserts SampleBuffer in the current pipe position and
     * advances to the next position.
     */
    SamplesPipe& operator<<(const SampleBuffer& buffer);

private:
    SamplesPipe(const SamplesPipe&) = delete;
    SamplesPipe& operator=(const SamplesPipe&) = delete;

    static void setLayout(const SampleLayout& layout);

    friend class RenderingServer;
    friend class TilePool;
    static int64_t sm_sampleSize;
    static int64_t sm_numSamples;
    static std::vector<std::pair<int, int>> sm_inputParameterIndices;
    static std::vector<std::pair<int, int>> sm_outputParameterIndices;
    static std::vector<std::pair<int, int>> sm_outputFeatureIndices;
    float* m_samples;
    float* m_currentSamplePtr;
    Point2l m_begin;
    Point2l m_end;
    int64_t m_width;
    int64_t m_informedNumSamples = 0;
    int64_t m_numSamples = 0;
};

} // namespace fbksd

#endif // SAMPLESPIPE_H