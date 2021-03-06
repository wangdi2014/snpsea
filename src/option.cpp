// Copyright (c) 2013-2014 Kamil Slowikowski
// See LICENSE for GPLv3 license.

#include "ezOptionParser.h"
#include "snpsea.h"

using namespace ez;

void Usage(ezOptionParser & opt)
{
    std::string usage;
    opt.getUsage(usage); //, 80, ez::ezOptionParser::Layout::INTERLEAVE);
    std::cout << usage;
};

int main(int argc, const char * argv[])
{
    ezOptionParser opt;

    opt.overview =
        "SNPsea: an algorithm to identify cell types, tissues, and pathways\n"
        "        affected by risk loci";
    opt.syntax = "    snpsea [OPTIONS]";
    opt.example =
        "    snpsea --snps file.txt               \\ # or  --snps random20\n"
        "           --gene-matrix file.gct.gz     \\\n"
        "           --null-snps file.txt          \\\n"
        "           --snp-intervals file.bed.gz   \\\n"
        "           --gene-intervals file.bed.gz  \\\n"
        "           --condition file.txt          \\\n"
        "           --out folder                  \\\n"
        "           --score single                \\\n"
        "           --slop 10e3                   \\\n"
        "           --threads 2                   \\\n"
        "           --null-snpsets 100            \\\n"
        "           --min-observations 25         \\\n"
        "           --max-iterations 1e6\n\n";
    opt.footer =
        "SNPsea " SNPSEA_VERSION " Copyright (C) 2013-2014 Kamil Slowikowski"
        " <slowikow@broadinstitute.org>\n"
        "This program is free and without warranty under the GPLv3 license.\n\n";

    // Don't put extra spaces between options.
    opt.doublespace = 0;

    opt.add(
        "", // Default.
        0, // Required?
        0, // Number of args expected.
        0, // Delimiter if expecting multiple args.
        "Display usage instructions.", // Help description.
        "-h",    // Flag token.
        "--help" // Flag token.
    );

    opt.add(
        "", // Default.
        0, // Required?
        0, // Number of args expected.
        0, // Delimiter if expecting multiple args.
        "Display version and exit.\n", // Help description.
        "-v",       // Flag token.
        "--version" // Flag token.
    );

    opt.add(
        "", // Default.
        0, // Required?
        1, // Number of args expected.
        0, // Delimiter if expecting multiple args.
        "Text file with SNPsea arguments.",
        "--args" // Flag token.
    );

    opt.add(
        "", // Default.
        1, // Required?
        1, // Number of args expected.
        0, // Delimiter if expecting multiple args.
        "Text file with SNP identifiers in the first column."
        " Instead of a file name, you may use 'randomN' with an integer N for"
        " a random SNP list of length N.",
        "--snps" // Flag token.
    );

    opt.add(
        "", // Default.
        1, // Required?
        1, // Number of args expected.
        0, // Delimiter if expecting multiple args.
        "Gene matrix file in GCT format. The Name column must contain the"
        " same gene identifiers as in --gene-intervals.",
        "--gene-matrix" // Flag token.
    );

    opt.add(
        "", // Default.
        1, // Required?
        1, // Number of args expected.
        0, // Delimiter if expecting multiple args.
        "BED file with gene intervals. The fourth column must contain the"
        " same gene identifiers as in --gene-matrix.",
        "--gene-intervals" // Flag token.
    );

    opt.add(
        "", // Default.
        1, // Required?
        1, // Number of args expected.
        0, // Delimiter if expecting multiple args.
        "BED file with all known SNP intervals. The fourth column must"
        " contain the same SNP identifiers as in --snps and --null-snps.",
        "--snp-intervals" // Flag token.
    );

    opt.add(
        "", // Default.
        1, // Required?
        1, // Number of args expected.
        0, // Delimiter if expecting multiple args.
        "Text file with SNP identifiers to sample when generating null"
        " matched or random SNP sets. These SNPs must be a subset of"
        " --snp-intervals.",
        "--null-snps" // Flag token.
    );

    opt.add(
        "", // Default.
        1, // Required?
        1, // Number of args expected.
        0, // Delimiter if expecting multiple args.
        "Create output files in this directory.\n\n", // Help description.
        "--out" // Flag token.
    );

    opt.add(
        "", // Default.
        0, // Required?
        1, // Number of args expected.
        0, // Delimiter if expecting multiple args.
        "Text file with a list of columns in --gene-matrix to condition on"
        " before calculating p-values. For each column in --gene-matrix we"
        " subtract its projection onto the columns listed in --condition.",
        "--condition" // Flag token.
    );

    ezOptionValidator * vU8 = new ezOptionValidator(ezOptionValidator::U8);
    opt.add(
        "10000", // Default.
        0, // Required?
        1, // Number of args expected.
        0, // Delimiter if expecting multiple args.
        "If a SNP overlaps no gene intervals, extend the SNP interval this"
        " many nucleotides further and try again.\n[default: 10000]",
        "--slop", // Flag token.
        vU8
    );

    opt.add(
        "single", // Default.
        0, // Required?
        1, // Number of args expected.
        0, // Delimiter if expecting multiple args.
        "Score each SNP locus with its 'single' most specific gene"
        " or the 'total' of all genes in the locus.\n[default: single]",
        "--score" // Flag token.
    );

    auto ge1 = new ezOptionValidator("s4", "ge", "1");
    opt.add(
        "1", // Default.
        0, // Required?
        1, // Number of args expected.
        0, // Delimiter if expecting multiple args.
        "Number of threads to use. [default: 1]",
        "--threads", // Flag token.
        ge1
    );

    auto ge0 = new ezOptionValidator("s4", "ge", "0");
    opt.add(
        "10", // Default.
        0, // Required?
        1, // Number of args expected.
        0, // Delimiter if expecting multiple args.
        "Generate a distribution of scores with N null matched SNP sets"
        " to evaluate type 1 error.\n[default: 0]",
        "--null-snpsets", // Flag token.
        ge0
    );

    opt.add(
        "25", // Default.
        0, // Required?
        1, // Number of args expected.
        0, // Delimiter if expecting multiple args.
        "Stop testing a column in --gene-matrix after observing this many"
        " null SNP sets with specificity scores greater or equal to those"
        " obtained with the SNPs in --snps. Increase this value to obtain"
        " more accurate p-values.\n[default: 25]",
        "--min-observations", // Flag token.
        ge1
    );

    opt.add(
        "1000", // Default.
        0, // Required?
        1, // Number of args expected.
        0, // Delimiter if expecting multiple args.
        "Maximum number of null SNP sets tested against each column in"
        " --gene-matrix. Increase this value to resolve small p-values."
        "\n[default: 10000]",
        "--max-iterations", // Flag token.
        ge1
    );

    // Read the options.
    opt.parse(argc, argv);

    if (opt.isSet("-h")) {
        Usage(opt);
        return 1;
    }

    if (opt.isSet("-v")) {
        std::cout << SNPSEA_VERSION << std::endl;
        return 1;
    }

    // Import options from one or more files that use # as comment char.
    if (opt.isSet("--args")) {
        std::vector< std::vector<std::string> > files;
        opt.get("--args")->getMultiStrings(files);

        for (int j = 0; j < files.size(); j++) {
            if (! opt.importFile(files[j][0].c_str(), '#')) {
                std::cerr << "ERROR: Failed to open file "
                          << files[j][0] << std::endl;
            }
        }
    }

    std::vector<std::string> badOptions;
    int i;
    if (!opt.gotRequired(badOptions)) {
        Usage(opt);
        for (i = 0; i < badOptions.size(); ++i) {
            std::cerr << "ERROR: Missing required option "
                      << badOptions[i] << ".\n";
        }
        return 1;
    }

    if (!opt.gotExpected(badOptions)) {
        Usage(opt);
        for (i = 0; i < badOptions.size(); ++i) {
            std::cerr << "ERROR: Got unexpected number of arguments for "
                      << badOptions[i] << ".\n";
        }
        return 1;
    }

    std::string
    user_snpset_file,
    gene_matrix_file,
    gene_intervals_file,
    snp_intervals_file,
    null_snps_file,
    condition_file,
    out_folder,
    score_method;

    opt.get("--snps")->getString(user_snpset_file);
    opt.get("--gene-matrix")->getString(gene_matrix_file);
    opt.get("--gene-intervals")->getString(gene_intervals_file);
    opt.get("--snp-intervals")->getString(snp_intervals_file);
    opt.get("--null-snps")->getString(null_snps_file);
    opt.get("--condition")->getString(condition_file);
    opt.get("--out")->getString(out_folder);
    opt.get("--score")->getString(score_method);

    // Ensure the files exist.
    // The argument may be a filename or a string like "random20".
    if (user_snpset_file.find("random") == 0) {
        // Grab the number, ensure it is above zero.
        std::string::size_type sz;
        int n = std::stoi(user_snpset_file.substr(6), &sz);
        if (n <= 0) {
            std::cerr << "ERROR: --snps " + user_snpset_file << std::endl;
            std::cerr << "Must be like: random20" << std::endl;
            exit(EXIT_FAILURE);
        }
    } else {
        // Otherwise, ensure the file exists.
        assert_file_exists(user_snpset_file);
    }
    assert_file_exists(gene_matrix_file);
    assert_file_exists(gene_intervals_file);
    assert_file_exists(snp_intervals_file);
    assert_file_exists(null_snps_file);
    // Optional.
    if (condition_file.length() > 0) {
        assert_file_exists(condition_file);
    }

    // Create the output directory.
    mkpath(out_folder);

    // Restrict the score methods.
    if (score_method[0] == 's') {
        score_method = "single";
    } else if (score_method[0] == 't') {
        score_method = "total";
    } else {
        std::cerr << "ERROR: --score" << score_method << std::endl;
        std::cerr << "Must be one of: single total" << std::endl;
        exit(EXIT_FAILURE);
    }

    int
    threads,
    slop;

    long
    null_snpset_replicates,
    min_observations,
    max_iterations;

    opt.get("--threads")->getInt(threads);
    opt.get("--null-snpsets")->getLong(null_snpset_replicates);
    opt.get("--min-observations")->getLong(min_observations);

    double
    slop_d,
    max_iterations_d;

    // Read double so we can pass things like "1e6" and "250e3".
    opt.get("--slop")->getDouble(slop_d);
    opt.get("--max-iterations")->getDouble(max_iterations_d);

    // Convert to proper types.
    slop = slop_d;
    max_iterations = max_iterations_d;

    if (max_iterations <= 0) {
        std::cerr << "ERROR: Invalid option: --max-iterations "
                  << max_iterations << std::endl
                  << "This option may not exceed 1e18.\n";
        exit(EXIT_FAILURE);
    }

    if (min_observations >= max_iterations || min_observations <= 0) {
        std::cerr << "ERROR: Invalid option: --min-observations "
                  << min_observations << std::endl;
        exit(EXIT_FAILURE);
    }

    // Export all of the options used.
    //std::string argsfile = out_folder + "/args.txt";
    //opt.exportFile(argsfile.c_str(), true);

    // Run the analysis.
    snpsea(
        user_snpset_file,
        gene_matrix_file,
        gene_intervals_file,
        snp_intervals_file,
        null_snps_file,
        condition_file,
        out_folder,
        score_method,
        slop,
        threads,
        null_snpset_replicates,
        min_observations,
        max_iterations
    );

    return 0;
}
