# -*- coding: utf-8 -*-
import click
from jinja2 import Environment, FileSystemLoader, select_autoescape
import pathlib


ALLOWED_ARCHITECTURES = ("cpu", "gpu")
ALLOWED_LAYOUT_MAPS = ("0,1,2", "0,2,1", "1,0,2", "1,2,0", "2,0,1", "2,1,0")
PRECISION_TO_CPP_TYPE = {64: "double", 32: "float"}


@click.command()
@click.option(
    "-a",
    "--architecture",
    type=str,
    help="Target architecture. Options: `cpu`, `gpu`. Default: `cpu`.",
    show_default=False,
    default="cpu",
)
@click.option(
    "-m",
    "--layout-map",
    type=str,
    help="Storage layout. Options: any permutation of {0, 1, 2}. Default: `0, 1, 2`.",
    show_default=False,
    default="0, 1, 2",
)
@click.option(
    "-p",
    "--precision",
    type=int,
    help="Floating point precision (in bits). Options: 64, 32. Default: 64.",
    show_default=False,
    default=64,
)
def main(architecture: str, layout_map: str, precision: int) -> None:
    assert architecture in ALLOWED_ARCHITECTURES
    layout_map = layout_map.replace(" ", "")
    assert layout_map in ALLOWED_LAYOUT_MAPS
    layout_map = layout_map.replace(",", ", ")
    assert precision in PRECISION_TO_CPP_TYPE
    cpp_dtype = PRECISION_TO_CPP_TYPE[precision]

    file_dir = pathlib.Path(__file__).parent.absolute()
    env = Environment(
        loader=FileSystemLoader(file_dir / "_templates"), autoescape=select_autoescape()
    )
    template = env.get_template("type_list.hpp.jinja")
    template.stream(architecture=architecture, layout_map=layout_map, cpp_dtype=cpp_dtype).dump(
        str(file_dir / "include/ghex/bindings/python/type_list.hpp")
    )


if __name__ == "__main__":
    main()
