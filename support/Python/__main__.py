# Distributed under the MIT License.
# See LICENSE.txt for details.

import click
import logging
import rich.logging
import rich.traceback

SPECTRE_VERSION = "@SPECTRE_VERSION@"


# Load subcommands lazily, i.e., only import the module when the subcommand is
# invoked. This is important so the CLI responds quickly.
class Cli(click.MultiCommand):
    def list_commands(self, ctx):
        return [
            "clean-output",
            "extract-dat",
            "extract-input",
            "generate-xdmf",
            "simplify-traces",
        ]

    def get_command(self, ctx, name):
        if name == "clean-output":
            from spectre.tools.CleanOutput import clean_output_command
            return clean_output_command
        elif name == "extract-dat":
            from spectre.IO.H5.ExtractDatFromH5 import extract_dat_command
            return extract_dat_command
        elif name == "extract-input":
            from spectre.IO.H5.ExtractInputSourceYamlFromH5 import (
                extract_input_source_from_h5_command)
            return extract_input_source_from_h5_command
        elif name == "generate-xdmf":
            from spectre.Visualization.GenerateXdmf import (
                generate_xdmf_command)
            return generate_xdmf_command
        elif name == "simplify-traces":
            from spectre.tools.CharmSimplifyTraces import (
                simplify_traces_command)
            return simplify_traces_command
        raise NotImplementedError(f"The command '{name}' is not implemented.")


# Set up CLI entry point
@click.group(context_settings=dict(help_option_names=["-h", "--help"]),
             help=f"SpECTRE version: {SPECTRE_VERSION}",
             cls=Cli)
@click.version_option(version=SPECTRE_VERSION, message="%(version)s")
@click.option('--debug', 'log_level', flag_value=logging.DEBUG)
@click.option('--silent', 'log_level', flag_value=logging.CRITICAL)
def cli(log_level):
    if log_level is None:
        log_level = logging.INFO
    # Configure logging
    logging.basicConfig(level=log_level,
                        format="%(message)s",
                        datefmt="[%X]",
                        handlers=[rich.logging.RichHandler()])
    # Format tracebacks with rich
    # - Suppress traceback entries from modules that we don't care about
    rich.traceback.install(
        show_locals=log_level <= logging.DEBUG,
        extra_lines=(3 if log_level <= logging.DEBUG else 0),
        suppress=[click])


if __name__ == "__main__":
    cli(prog_name="spectre")
