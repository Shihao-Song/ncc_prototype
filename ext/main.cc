#include "util/args.hh"
#include "unroll/unroll.hh"

// TODO, fanin -> should be optional
int main(int argc, char **argv)
{
    EXT::Argument args(argc, argv);

    EXT::Unrolling::Model model(args.getConnFile(),
                                args.getSpikeFile());

    if (auto fanin = args.getFanin();
        fanin != EXT::Argument::INVALID_FANIN)
    {
        model.setFanin(args.getFanin());
        model.unroll();
    }

    if (auto &unrolled_ir = args.getUnrolledIROutputFile();
        unrolled_ir != "N/A")
    {
        model.outputUnrolledIR(unrolled_ir);
    }

    if (auto &parent_neu_out = args.getUnrolledParentOutputFile();
        parent_neu_out != "N/A")
    {
        model.parentNeuronOutput(parent_neu_out);
    }

    return 0;
}
