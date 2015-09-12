/*
 * qs_QuantitySequence.h
 *
 *  Created on: 30/05/2015
 *      Author: jim
 */

#ifndef TNZ_QUANTITY_H_
#define TNZ_QUANTITY_H_

#include "BitSink.h"

#include <stdint.h>

#include <map>
#include <memory>
#include <ostream>
#include <string>
#include <vector>

namespace qs {

/*
 * "Quantity is a property that can exist as a magnitude or multitude" [Wikipedia]
 * e.g. speed, latitude, longitude, acceleration, a count, a percentage, an enumeration.
 *
 * QuantityInfo: information about a one-dimensional quantity.
 * QStep: quantization step size (encoding)
 * QuantitiesSequence: class for encoding a sequence of quantities
 */
struct QStep {
    uint8_t sig;
    int8_t  exp;
    QStep(uint8_t sig=0, int8_t exp=0) : sig(sig), exp(exp) {}
};

struct QuantityInfo {
    std::string name;
    std::string unit;
    QStep qStep;
    QuantityInfo(const std::string& name="", const std::string& unit="", const QStep& qStep=QStep())
        : name(name), unit(unit), qStep(qStep) {}
};
class IntCoder;
class IntPredictor;
class QuantitiesSequence
{
    public:
        QuantitiesSequence(const std::vector<QuantityInfo>& qInfos);
        ~QuantitiesSequence();

        void push(const std::vector<double>& quantities);

        std::vector<uint8_t> getCode() const;

    private:
        std::vector<QuantityInfo> qInfos;
        std::vector<std::shared_ptr<IntCoder> > intCoders;
        std::vector<std::shared_ptr<ByteBufferSink> > byteSinks;
        std::vector<BitSink> bitSinks;
        std::vector<std::shared_ptr<IntPredictor> > intPredictors;
        std::vector<double> qMuls;
        uint32_t numVals;
};

extern const char** getStdQuantities(); // Table with up to 255 standard (enumerated) channel names
extern const uint8_t STD_QUANTITY_NOT_PRESENT; // Value indicating quantity not in standard quantities list
extern uint8_t getStdQuantityIdx(const std::string& name);

extern std::ostream& operator<<(std::ostream& os, const QStep& qStep);
extern std::ostream& operator<<(std::ostream& os, const QuantityInfo& ci);
extern std::ostream& operator<<(std::ostream& os, const std::vector<QuantityInfo>& quantityInfos);

} // namespace qs

#endif /* TNZ_QUANTITY_H_ */
