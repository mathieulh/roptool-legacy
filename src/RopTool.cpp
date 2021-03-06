// roptool
#include "RopTool.h"
#include "Parser.h"
#include "FolderTarget.h"
#include "CodeGenerator.h"
#include "Program.h"
#include "Debug.h"

// std
#include <iostream>
#include <fstream>

// boost
#include <boost/bind.hpp>

// namespace for ease of reading (and writing)
namespace po = boost::program_options;

void RopTool::set_target(const std::string& target)
{
    // create target
    this->target.reset(new FolderTarget(target));
}

void RopTool::set_source(const std::string& source)
{
    // \TODO: verify source
    this->source = source;
}

void RopTool::set_output(const std::string& output)
{
    this->output = output;
}

void RopTool::set_base(const std::string& address)
{
    // \TODO: error check
    base_address = std::stoull(address, nullptr, 0);
}

void RopTool::set_sled_length(const std::string& sled_length)
{
    m_sled_length = std::stoul(sled_length, nullptr, 0);
}

RopTool::cmd_options RopTool::get_options(void)
{
    // check if there are any options already created by this function
    if (m_options)
    {
        // options already set, just return
        return m_options;
    }
    
    // create new options
    m_options.reset(new po::options_description());
    
    m_options->add_options()
        ("help,h", "Show this help dialog.")
        ("verbose,v", "Show verbose output.")
        ("sled,n", po::value<std::string>()->composing()->notifier(boost::bind(&RopTool::set_sled_length, this, _1)), "Size of nop sled.")
        ("link,x", po::value<std::string>()->composing()->notifier(boost::bind(&RopTool::set_base, this, _1)), "The address to link the data section.")
        ("target,t", po::value<std::string>()->composing()->notifier(boost::bind(&RopTool::set_target, this, _1)), "Path to the target to build against.")
        ("source,s", po::value<std::string>()->composing()->notifier(boost::bind(&RopTool::set_source, this, _1)), "Source ropscript file to compile.")
        ("output,o", po::value<std::string>()->composing()->notifier(boost::bind(&RopTool::set_output, this, _1)), "Path to output file.");
    return m_options;
}

int RopTool::start(int argc, char *argv[])
{
    // get commandline options
    cmd_options options = get_options();
    
    try
    {
        // parse the command line
        po::store(po::parse_command_line(argc, argv, *options.get()), m_vm);
        po::notify(m_vm);
        
        // check for no args or the help
        if (m_vm.size() == 0 || m_vm.count("help"))
        {
            // show help
            std::cout << *options.get() << std::endl;
            return 1;
        }
        
        // check for a target
        if (!m_vm.count("target"))
        {
            // targets are required
            std::cerr << "A target is required." << std::endl;
            return 1;
        }
        
        // check for source
        if (!m_vm.count("source"))
        {
            // need a source file
            std::cerr << "A source file is required." << std::endl;
            return 1;
        }
        
        // check for output
        if (!m_vm.count("output"))
        {
            // requre an output
            std::cerr << "An output file is required." << std::endl;
            return 1;
        }
        
        RopScriptShared ast = parse(source.c_str(), target->manifest()->arch_bitlen());
        
        CodeGenerator generator;
        ProgramPtr exec = generator.compile(ast, target);
        
        if (!m_vm.count("link"))
        {
            base_address = 0xDAEEDAEEDAEEDAEELL;
            std::cout << "No link address specified, using: 0x" << std::hex << base_address << std::endl;
        }
        
        if (m_vm.count("sled"))
        {
            exec->addSled(m_sled_length);
        }
        
        exec->data().setBase(base_address);
        std::ofstream out(output);
        exec->write(out);
        out.close();
    }
 
    // catch any exceptions
    catch(std::exception& e) 
    {
        // display the exception
        std::cerr << "Error: " << e.what() << std::endl;
        std::cerr << "Use --help for usage information." << std::endl;
        return 1;
    }
    
    return 0;
}

int main(int argc, char *argv[])
{
    RopTool roptool;
    return roptool.start(argc, argv);
}
