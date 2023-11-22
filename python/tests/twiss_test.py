from thor_scsi.utils import canonical_variables_mapping_default
from thor_scsi.utils.linear_optics import compute_Twiss_along_lattice
from thor_scsi.factory import accelerator_from_config
from thor_scsi import lib as tslib
import gtpsa
import pytest
from pathlib import Path
import os.path

desc = gtpsa.desc(6, 1)

path = (
    Path(os.path.dirname(__file__))
    / "lattices"
    / "b2_stduser_beamports_blm_tracy_corr.lat"
)


def create_acc():
    acc = accelerator_from_config(path)
    acc.set_log_level(tslib.accelerator_log_level.warning)
    return acc


def test_twiss_without_radiation():
    """Functions as used by digital twin"""
    n_dof = 2
    calc_config = tslib.ConfigType()
    calc_config.radiation = False
    calc_config.Cavity_on = False
    acc = create_acc()
    compute_Twiss_along_lattice(
        n_dof, acc, calc_config, desc=desc, mapping=canonical_variables_mapping_default
    )


@pytest.mark.skip
def test_twiss_with_radiation():
    """Functions as used by digital twin

    just check that it runs
    """
    calc_config = tslib.ConfigType()
    n_dof = 3
    calc_config.radiation = True
    calc_config.Cavity_on = True
    calc_config.Energy = 2.7
    acc = create_acc()
    compute_Twiss_along_lattice(
        n_dof, acc, calc_config, desc=desc, mapping=canonical_variables_mapping_default
    )
