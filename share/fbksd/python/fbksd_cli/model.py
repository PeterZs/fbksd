# Copyright (c) 2019 Jonas Deyson
#
# This software is released under the MIT License.
#
# You should have received a copy of the MIT License
# along with this program. If not, see <https://opensource.org/licenses/MIT>

import uuid
import os

class BaseModel():
    def __init__(self, obj_id):
        self.id = obj_id
        self.uuid = uuid.uuid4()


class Technique(BaseModel):
    def __init__(self, obj_id):
        super().__init__(obj_id)
        self.name = ''
        self.full_name = ''
        self.comment = ''
        self.citation = ''
        self.versions = []

    def get_version(self, tag):
        for v in self.versions:
            if v.tag == tag:
                return v 
        return None


class Filter(Technique):
    __nextId = 1

    def __init__(self):
        super().__init__(Filter.__nextId)
        Filter.__nextId += 1


class Sampler(Technique):
    __nextId = 1

    def __init__(self):
        super().__init__(Sampler.__nextId)
        Sampler.__nextId += 1


class TechniqueVersion(BaseModel):
    def __init__(self, obj_id):
        super().__init__(obj_id)
        self.technique = None
        self.tag = ''
        self.executable = ''
        self.message = ''
        self.status = ''
        self.results = [] # Result[]

    def has_result(self, scene, spp):
        return any([True for result in self.results if result.scene is scene and result.spp == spp])

    def get_result(self, scene, spp):
        results = [result for result in self.results if result.scene is scene and result.spp == spp]
        if results:
            return results[0]
        else:
            return None

    def get_results(self, scene):
        r_results = []
        for r in self.results:
            if r.scene is scene:
                r_results.append(r)
        return r_results

    def get_name(self):
        return '{}-{}'.format(self.technique.name, self.tag)


class FilterVersion(TechniqueVersion):
    __nextId = 1

    def __init__(self):
        super().__init__(FilterVersion.__nextId)
        FilterVersion.__nextId += 1


class SamplerVersion(TechniqueVersion):
    __nextId = 1

    def __init__(self):
        super().__init__(SamplerVersion.__nextId)
        SamplerVersion.__nextId += 1


class Renderer(BaseModel):
    __nextId = 1

    def __init__(self):
        super().__init__(Renderer.__nextId)
        Renderer.__nextId += 1

        self.name = ''
        self.path = ''
        self.is_ready = False
        self.scenes = [] # Scene[]


class Scene(BaseModel):
    __nextId = 1

    def __init__(self):
        super().__init__(Scene.__nextId)
        Scene.__nextId += 1

        self.name = ''
        self.path = ''
        self.ground_truth = '' # reference .exr image (relative to scenes/<renderer>/ folder)
        self.dof_w = 0.0 # depth-of-field weight
        self.mb_w = 0.0 # motion blur
        self.ss_w = 0.0 # soft shadow
        self.glossy_w = 0.0 # glossy
        self.gi_w = 0.0 # global illumination
        self.renderer = None # Renderer
        self.regions = [] # ImageRegion

    def get_name(self):
        return self.name

    def get_reference(self):
        """Returns the reference image relative to the scenes folder."""
        return os.path.join(self.renderer.name, self.ground_truth)


class ConfigScene:
    def __init__(self, scene, spps):
        self.scene = scene
        self.spps = spps

    def get_name(self):
        return self.scene.name


class ImageRegion(BaseModel):
    __nextId = 1

    def __init__(self):
        super().__init__(ImageRegion.__nextId)
        ImageRegion.__nextId += 1

        self.xmin = 0
        self.ymin = 0
        self.xmax = 0
        self.ymax = 0
        # Weights for each noise source (0.0 - 1.0)
        self.dof_w = 0.0 # depth-of-field weight
        self.mb_w = 0.0 # motion blur
        self.ss_w = 0.0 # soft shadow
        self.glossy_w = 0.0 # glossy
        self.gi_w = 0.0 # global illumination


class IQAMetric():
    def __init__(self, acronym, name, reference, lower_is_better, has_error_map, command):
        self.acronym = acronym
        self.name = name
        self.reference = reference
        self.lower_is_better = lower_is_better
        self.has_error_map = has_error_map
        self.command = command # command is the executable path for non-built-in methods


class RegionError(BaseModel):
    __nextId = 1

    def __init__(self):
        super().__init__(RegionError.__nextId)
        RegionError.__nextId += 1
        self.metrics = {}
        self.region = None # ImageRegion
        self.result = None # Result


class TechniqueResult(BaseModel):
    def __init__(self, obj_id):
        super().__init__(obj_id)

        self.scene = None
        self.spp = 0
        self.exec_time = 0
        self.metrics = {}
        self.aborted = False
        self.region_errors = [] # RegionError[]


class Result(TechniqueResult):
    __nextId = 1

    def __init__(self):
        super().__init__(Result.__nextId)
        Result.__nextId += 1
        self.filter_version = None # SamplerVersion


class SamplerResult(TechniqueResult):
    __nextId = 1

    def __init__(self):
        super().__init__(SamplerResult.__nextId)
        SamplerResult.__nextId += 1
        self.sampler_version = None # FilterVersion


class PersistentState:
    def __init__(self):
        pass
