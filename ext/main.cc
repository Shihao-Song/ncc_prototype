#include "unroll/unroll.hh"

// TODO, argument -> new file
// TODO, fanin -> should be optional
int main(int argc, char **argv)
{
    Unrolling::Argument args(argc, argv);

    Unrolling::Model model(args.getConnFile(),
                           args.getSpikeFile());

    model.setFanin(args.getFanin());
    model.unroll();
    model.output(args.getOutputFile());

    if (auto &parent_neu_out = args.getParentNeuronOutputFile();
        parent_neu_out != "N/A")
    {
        model.parentNeuronOutput(parent_neu_out);
    }

    if (auto &debug_out = args.getDebugOutputFile();
        debug_out != "N/A")
    {
        model.debugOutput(debug_out);
    }

    return 0;
}
