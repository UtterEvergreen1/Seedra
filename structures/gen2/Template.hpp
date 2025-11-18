#pragma once

#include "common/Pos3DTemplate.hpp"
#include "common/nbt.hpp"
#include "lce/blocks/block.hpp"
#include "terrain/World.hpp"
#include <list>
#include <utility>

class Template {
public:
    
    class BlockInfo {
    public:
        const Pos3D pos;
        const lce::BlockState blockState;
        const NBTCompound tileentityData;
    
        BlockInfo(Pos3D posIn, lce::BlockState stateIn, NBTCompound compoundIn)
                : pos(posIn), blockState(stateIn), tileentityData(std::move(compoundIn)) {}
    };
    
    class EntityInfo {
    public:
        const Pos3DTemplate<double> pos;
        const Pos3D blockPos;
        const NBTCompound entityData;
    
        EntityInfo(Pos3DTemplate<double> vecIn, Pos3D posIn, NBTCompound compoundIn)
            : pos(vecIn), blockPos(posIn), entityData(std::move(compoundIn)) {}
    };
    
    
private:
    const std::list<Template::BlockInfo> blocks = std::list<Template::BlockInfo>();
    const std::list<Template::EntityInfo> entities = std::list<Template::EntityInfo>();

    Pos3D size = {0, 0, 0};

    std::string author = "?";

                                                                                            
public:
    
    Pos3D getSize() {
        return this->size;
    }

    void setAuthor(const std::string& authorIn) {
        this->author = authorIn;
    }

    std::string getAuthor() {
        return this->author;
    }

    void takeBlocksFromWorld(World worldIn, Pos3D startPos, Pos3D endPos, bool takeEntities, lce::BlockState toIgnore) {
        if (endPos.getX() >= 1 && endPos.getY() >= 1 && endPos.getZ() >= 1) {
            Pos3D blockpos = startPos.add(endPos).add(-1, -1, -1);
            std::list<Template::BlockInfo> list = std::list<Template::BlockInfo>();
            std::list<Template::BlockInfo> list1 = std::list<Template::BlockInfo>();
            std::list<Template::BlockInfo> list2 = std::list<Template::BlockInfo>();
            Pos3D blockpos1 =
                    Pos3D(std::min(startPos.getX(), blockpos.getX()), std::min(startPos.getY(), blockpos.getY()),
                                 std::min(startPos.getZ(), blockpos.getZ()));
            Pos3D blockpos2 =
                    Pos3D(std::max(startPos.getX(), blockpos.getX()), std::max(startPos.getY(), blockpos.getY()),
                                 std::max(startPos.getZ(), blockpos.getZ()));
            this->size = endPos;

            for (BlockPos.MutablePos3D blockpos$mutableblockpos: BlockPos.getAllInBoxMutable(blockpos1, blockpos2)) {
                Pos3D blockpos3 = blockpos$mutableblockpos.subtract(blockpos1);
                lce::BlockState iblockstate = worldIn.getBlockState(blockpos$mutableblockpos);

                if (toIgnore == null || toIgnore != iblockstate.getBlock()) {
                    TileEntity tileentity = worldIn.getTileEntity(blockpos$mutableblockpos);

                    if (tileentity != null) {
                        NBTCompound nbttagcompound = tileentity.writeToNBT(new NBTCompound());
                        nbttagcompound.removeTag("x");
                        nbttagcompound.removeTag("y");
                        nbttagcompound.removeTag("z");
                        list1.add(new Template::BlockInfo(blockpos3, iblockstate, nbttagcompound));
                    } else if (!iblockstate.isFullBlock() && !iblockstate.isFullCube()) {
                        list2.add(new Template::BlockInfo(blockpos3, iblockstate, (NBTCompound) null));
                    } else {
                        list.add(new Template::BlockInfo(blockpos3, iblockstate, (NBTCompound) null));
                    }
                }
            }

            this->blocks.clear();
            this->blocks.addAll(list);
            this->blocks.addAll(list1);
            this->blocks.addAll(list2);

            if (takeEntities) {
                this->takeEntitiesFromWorld(worldIn, blockpos1, blockpos2.add(1, 1, 1));
            } else {
                this->entities.clear();
            }
        }
    }

private:
    void takeEntitiesFromWorld(World worldIn, Pos3D startPos, Pos3D endPos) {
        std::list<Entity> list = worldIn.<Entity>getEntitiesWithinAABB(
                Entity.class, new AxisAlignedBB(startPos, endPos), new Predicate<Entity>() {
                public
                    bool apply(@Nullable Entity entityCandidate) {
                        return !(entityCandidate instanceof EntityPlayer);
                    }
                });
        this->entities.clear();

        for (Entity entity: list) {
            Pos3DTemplate<double> vec3d = new Pos3DTemplate<double>(entity.posX - (double) startPos.getX(), entity.posY - (double) startPos.getY(),
                                    entity.posZ - (double) startPos.getZ());
            NBTCompound nbttagcompound = new NBTCompound();
            entity.writeToNBTOptional(nbttagcompound);
            Pos3D blockpos;

            if (entity instanceof EntityPainting) {
                blockpos = ((EntityPainting) entity).getHangingPosition().subtract(startPos);
            } else {
                blockpos = Pos3D(vec3d);
            }

            this->entities.add(new Template::EntityInfo(vec3d, blockpos, nbttagcompound));
        }
    }

public:
    Map<BlockPos, String> getDataBlocks(Pos3D pos, PlacementSettings placementIn) {
        Map<BlockPos, String> map = Maps.<BlockPos, String>newHashMap();
        StructureBoundingBox structureboundingbox = placementIn.getBoundingBox();

        for (Template::BlockInfo template$blockinfo: this->blocks) {
            Pos3D blockpos = transformedBlockPos(placementIn, template$blockinfo.pos).add(pos);

            if (structureboundingbox == null || structureboundingbox.isVecInside(blockpos)) {
                lce::BlockState iblockstate = template$blockinfo.blockState;

                if (iblockstate.getBlock() == Blocks.STRUCTURE_BLOCK && template$blockinfo.tileentityData != null) {
                    TileEntityStructure.Mode tileentitystructure$mode =
                            TileEntityStructure.Mode.valueOf(template$blockinfo.tileentityData.getString("mode"));

                    if (tileentitystructure$mode == TileEntityStructure.Mode.DATA) {
                        map.put(blockpos, template$blockinfo.tileentityData.getString("metadata"));
                    }
                }
            }
        }

        return map;
    }

    Pos3D calculateConnectedPos(PlacementSettings fromSettings, Pos3D fromPos, PlacementSettings toSettings, Pos3D toPos) {
        Pos3D blockpos = transformedBlockPos(fromSettings, fromPos);
        Pos3D blockpos1 = transformedBlockPos(toSettings, toPos);
        return blockpos.subtract(blockpos1);
    }

    static Pos3D transformedBlockPos(PlacementSettings settings, Pos3D localPos) {
        return transformedBlockPos(localPos, settings.getMirror(), settings.getRotation());
    }

    void addBlocksToWorldChunk(World worldIn, Pos3D pos, PlacementSettings placementIn) {
        placementIn.setBoundingBoxFromChunk();
        this->addBlocksToWorld(worldIn, pos, placementIn);
    }

    void addBlocksToWorld(World worldIn, Pos3D pos, PlacementSettings placementIn) {
        this->addBlocksToWorld(worldIn, pos, new BlockRotationProcessor(pos, placementIn), placementIn, 2);
    }

    void addBlocksToWorld(World worldIn, Pos3D pos, PlacementSettings placementIn, int flags) {
        this->addBlocksToWorld(worldIn, pos, new BlockRotationProcessor(pos, placementIn), placementIn, flags);
    }

    void addBlocksToWorld(World world, Pos3D origin, ITemplateProcessor processor, PlacementSettings settings, int flags) {
        if ((!this->blocks.isEmpty() || !settings.getIgnoreEntities() && !this->entities.isEmpty()) &&
            this->size.getX() >= 1 && this->size.getY() >= 1 && this->size.getZ() >= 1) {
            Block block = settings.getReplacedBlock();
            StructureBoundingBox structureboundingbox = settings.getBoundingBox();

            for (Template::BlockInfo template$blockinfo: this->blocks) {
                Pos3D blockpos = transformedBlockPos(settings, template$blockinfo.pos).add(origin);
                Template::BlockInfo template$blockinfo1 =
                        processor != null ? processor.processBlock(world, blockpos, template$blockinfo)
                                          : template$blockinfo;

                if (template$blockinfo1 != null) {
                    Block block1 = template$blockinfo1.blockState.getBlock();

                    if ((block == null || block != block1) &&
                        (!settings.getIgnoreStructureBlock() || block1 != Blocks.STRUCTURE_BLOCK) &&
                        (structureboundingbox == null || structureboundingbox.isVecInside(blockpos))) {
                        lce::BlockState iblockstate = template$blockinfo1.blockState.withMirror(settings.getMirror());
                        lce::BlockState iblockstate1 = iblockstate.withRotation(settings.getRotation());

                        if (template$blockinfo1.tileentityData != null) {
                            TileEntity tileentity = world.getTileEntity(blockpos);

                            if (tileentity != null) {
                                if (tileentity instanceof IInventory) { ((IInventory) tileentity).clear(); }

                                world.setBlockState(blockpos, Blocks.BARRIER.getDefaultState(), 4);
                            }
                        }

                        if (world.setBlockState(blockpos, iblockstate1, flags) &&
                            template$blockinfo1.tileentityData != null) {
                            TileEntity tileentity2 = world.getTileEntity(blockpos);

                            if (tileentity2 != null) {
                                template$blockinfo1.tileentityData.setInteger("x", blockpos.getX());
                                template$blockinfo1.tileentityData.setInteger("y", blockpos.getY());
                                template$blockinfo1.tileentityData.setInteger("z", blockpos.getZ());
                                tileentity2.readFromNBT(template$blockinfo1.tileentityData);
                                tileentity2.mirror(settings.getMirror());
                                tileentity2.rotate(settings.getRotation());
                            }
                        }
                    }
                }
            }

            for (Template::BlockInfo template$blockinfo2: this->blocks) {
                if (block == null || block != template$blockinfo2.blockState.getBlock()) {
                    Pos3D blockpos1 = transformedBlockPos(settings, template$blockinfo2.pos).add(origin);

                    if (structureboundingbox == null || structureboundingbox.isVecInside(blockpos1)) {
                        world.notifyNeighborsRespectDebug(blockpos1, template$blockinfo2.blockState.getBlock(), false);

                        if (template$blockinfo2.tileentityData != null) {
                            TileEntity tileentity1 = world.getTileEntity(blockpos1);

                            if (tileentity1 != null) { tileentity1.markDirty(); }
                        }
                    }
                }
            }

            if (!settings.getIgnoreEntities()) {
                this->addEntitiesToWorld(world, origin, settings.getMirror(), settings.getRotation(),
                                        structureboundingbox);
            }
        }
    }

private:
    void addEntitiesToWorld(World worldIn, Pos3D pos, Mirror mirrorIn, Rotation rotationIn, BoundingBox aabb) {
        for (Template::EntityInfo template$entityinfo: this->entities) {
            Pos3D blockpos = transformedBlockPos(template$entityinfo.blockPos, mirrorIn, rotationIn).add(pos);

            if (/*aabb == null || */aabb.isVecInside(blockpos)) {
                NBTCompound nbttagcompound = template$entityinfo.entityData;
                Pos3DTemplate<double> vec3d =
                        transformedBlockPos<double>(template$entityinfo.pos, mirrorIn, rotationIn);
                Pos3DTemplate<double> vec3d1 = vec3d.addVector((double) pos.getX(), (double) pos.getY(), (double) pos.getZ());
                NBTList nbttaglist = new NBTList();
                nbttaglist.appendTag(new NBTTagDouble(vec3d1.xCoord));
                nbttaglist.appendTag(new NBTTagDouble(vec3d1.yCoord));
                nbttaglist.appendTag(new NBTTagDouble(vec3d1.zCoord));
                nbttagcompound.setTag("Pos", nbttaglist);
                nbttagcompound.setUniqueId("UUID", UUID.randomUUID());
                Entity entity;

                try {
                    entity = EntityList.createEntityFromNBT(nbttagcompound, worldIn);
                } catch (Exception var15) { entity = null; }

                if (entity != null) {
                    float f = entity.getMirroredYaw(mirrorIn);
                    f = f + (entity.rotationYaw - entity.getRotatedYaw(rotationIn));
                    entity.setLocationAndAngles(vec3d1.xCoord, vec3d1.yCoord, vec3d1.zCoord, f, entity.rotationPitch);
                    worldIn.spawnEntityInWorld(entity);
                }
            }
        }
    }
public:
    Pos3D transformedSize(Rotation rotationIn) {
        switch (rotationIn.type) {
            case Rotation::Type::COUNTERCLOCKWISE_90:
            case Rotation::Type::CLOCKWISE_90:
                return Pos3D(this->size.getZ(), this->size.getY(), this->size.getX());

            default:
                return this->size;
        }
    }
private:
    static Pos3D transformedBlockPos(Pos3D pos, Mirror mirrorIn, Rotation rotationIn) {
        int x = pos.getX();
        int y = pos.getY();
        int z = pos.getZ();
        bool flag = true;

        switch (mirrorIn.type) {
            case Mirror::Type::LEFT_RIGHT:
                z = -z;
                break;

            case Mirror::Type::FRONT_BACK:
                x = -x;
                break;

            default:
                flag = false;
        }

        switch (rotationIn.type) {
            case Rotation::Type::COUNTERCLOCKWISE_90:
                return Pos3D(z, y, -x);

            case Rotation::Type::CLOCKWISE_90:
                return Pos3D(-z, y, x);

            case Rotation::Type::CLOCKWISE_180:
                return Pos3D(-x, y, -z);

            default:
                return flag ? Pos3D(x, y, z) : pos;
        }
    }

    static Pos3DTemplate<double> transformedBlockPos(Pos3DTemplate<double> vec, Mirror mirrorIn, Rotation rotationIn) {
        double dx = vec.x;
        double dy = vec.y;
        double dz = vec.z;
        bool flag = true;

        switch (mirrorIn.type) {
            case Mirror::Type::LEFT_RIGHT:
                dz = 1.0 - dz;
                break;

            case Mirror::Type::FRONT_BACK:
                dx = 1.0 - dx;
                break;

            default:
                flag = false;
        }

        switch (rotationIn.type) {
            case Rotation::Type::COUNTERCLOCKWISE_90:
                return Pos3DTemplate<double>(dz, dy, 1.0 - dx);

            case Rotation::Type::CLOCKWISE_90:
                return Pos3DTemplate<double>(1.0 - dz, dy, dx);

            case Rotation::Type::CLOCKWISE_180:
                return Pos3DTemplate<double>(1.0 - dx, dy, 1.0 - dz);

            default:
                return flag ? Pos3DTemplate<double>(dx, dy, dz) : vec;
        }
    }
public:
    Pos3D getZeroPositionWithTransform(Pos3D origin, Mirror mirror, Rotation rotation) {
        return transformOrigin(origin, mirror, rotation, this->getSize().getX(), this->getSize().getZ());
    }

    static Pos3D transformOrigin(Pos3D origin, Mirror mirror, Rotation rotation, int width, int depth) {
        --width;
        --depth;
        int i = mirror == Mirror::FRONT_BACK ? width : 0;
        int j = mirror == Mirror::LEFT_RIGHT ? depth : 0;
        Pos3D blockpos = origin;

        switch (rotation.type) {
            case Rotation::Type::COUNTERCLOCKWISE_90:
                blockpos = origin.add(j, 0, width - i);
                break;

            case Rotation::Type::CLOCKWISE_90:
                blockpos = origin.add(depth - j, 0, i);
                break;

            case Rotation::Type::CLOCKWISE_180:
                blockpos = origin.add(width - i, 0, depth - j);
                break;

            case Rotation::Type::NONE:
                blockpos = origin.add(i, 0, j);
        }

        return blockpos;
    }

    static void registerTemplateDataFixers(DataFixer dataFixer) {
        dataFixer.registerWalker(FixTypes.STRUCTURE, new IDataWalker() {
            public NBTCompound process(IDataFixer fixer, NBTCompound compound, int versionIn) {
                if (compound.hasKey("entities", 9)) {
                    NBTList nbttaglist = compound.getTagList("entities", 10);

                    for (int i = 0; i < nbttaglist.tagCount(); ++i) {
            NBTCompound nbttagcompound = (NBTCompound) nbttaglist.get(i);

            if (nbttagcompound.hasKey("nbt", 10)) {
                nbttagcompound.setTag("nbt",
                                      fixer.process(FixTypes.ENTITY, nbttagcompound.getCompoundTag("nbt"), versionIn));
            }
                    }
    }

    if (compound.hasKey("blocks", 9)) {
        NBTList nbttaglist1 = compound.getTagList("blocks", 10);

        for (int j = 0; j < nbttaglist1.tagCount(); ++j) {
            NBTCompound nbttagcompound1 = (NBTCompound) nbttaglist1.get(j);

            if (nbttagcompound1.hasKey("nbt", 10)) {
                nbttagcompound1.setTag(
                        "nbt", fixer.process(FixTypes.BLOCK_ENTITY, nbttagcompound1.getCompoundTag("nbt"), versionIn));
            }
        }
    }

    return compound;
}
});}

public:
    NBTCompound writeToNBT(NBTCompound nbt) {
        Template::BasicPalette template$basicpalette = new Template::BasicPalette();
        NBTList nbttaglist = new NBTList();
    
        for (Template::BlockInfo template$blockinfo: this->blocks) {
            NBTCompound nbttagcompound = new NBTCompound();
            nbttagcompound.setTag("pos", this->writeInts(template$blockinfo.pos.getX(), template$blockinfo.pos.getY(),
                                                        template$blockinfo.pos.getZ()));
            nbttagcompound.setInteger("state", template$basicpalette.idFor(template$blockinfo.blockState));
    
            if (template$blockinfo.tileentityData != null) {
                nbttagcompound.setTag("nbt", template$blockinfo.tileentityData);
            }
    
            nbttaglist.appendTag(nbttagcompound);
        }
    
        NBTList nbttaglist1 = new NBTList();
    
        for (Template::EntityInfo template$entityinfo: this->entities) {
            NBTCompound nbttagcompound1 = new NBTCompound();
            nbttagcompound1.setTag("pos", this->writeDoubles(template$entityinfo.pos.xCoord, template$entityinfo.pos.yCoord,
                                                            template$entityinfo.pos.zCoord));
            nbttagcompound1.setTag("blockPos",
                                   this->writeInts(template$entityinfo.blockPos.getX(), template$entityinfo.blockPos.getY(),
                                                  template$entityinfo.blockPos.getZ()));
    
            if (template$entityinfo.entityData != null) { nbttagcompound1.setTag("nbt", template$entityinfo.entityData); }
    
            nbttaglist1.appendTag(nbttagcompound1);
        }
    
        NBTList nbttaglist2 = new NBTList();
    
        for (lce::BlockState iblockstate: template$basicpalette) {
            nbttaglist2.appendTag(NBTUtil.writeBlockState(new NBTCompound(), iblockstate));
        }
    
        nbt.setTag("palette", nbttaglist2);
        nbt.setTag("blocks", nbttaglist);
        nbt.setTag("entities", nbttaglist1);
        nbt.setTag("size", this->writeInts(this->size.getX(), this->size.getY(), this->size.getZ()));
        nbt.setString("author", this->author);
        nbt.setInteger("DataVersion", 1139);
        return nbt;
    }

    void read(NBTCompound& compound) {
        this->blocks.clear();
        this->entities.clear();
        NBTList nbttaglist = compound.getTagList("size", 3);
        this->size = Pos3D(nbttaglist.getIntAt(0), nbttaglist.getIntAt(1), nbttaglist.getIntAt(2));
        this->author = compound.getString("author");
        Template.BasicPalette template$basicpalette = new Template.BasicPalette();
        NBTList nbttaglist1 = compound.getTagList("palette", 10);
    
        for (int i = 0; i < nbttaglist1.tagCount(); ++i) {
            template$basicpalette.addMapping(NBTUtil.readBlockState(nbttaglist1.getCompoundTagAt(i)), i);
        }
    
        NBTList nbttaglist3 = compound.getTagList("blocks", 10);
    
        for (int j = 0; j < nbttaglist3.tagCount(); ++j) {
            NBTCompound nbttagcompound = nbttaglist3.getCompoundTagAt(j);
            NBTList nbttaglist2 = nbttagcompound.getTagList("pos", 3);
            Pos3D blockpos = Pos3D(nbttaglist2.getIntAt(0), nbttaglist2.getIntAt(1), nbttaglist2.getIntAt(2));
            lce::BlockState iblockstate = template$basicpalette.stateFor(nbttagcompound.getInteger("state"));
            NBTCompound nbttagcompound1;
    
            if (nbttagcompound.hasKey("nbt")) {
                nbttagcompound1 = nbttagcompound.getCompoundTag("nbt");
            } else {
                nbttagcompound1 = null;
            }
    
            this->blocks.add(new Template::BlockInfo(blockpos, iblockstate, nbttagcompound1));
        }
    
        NBTList nbttaglist4 = compound.getTagList("entities", 10);
    
        for (int k = 0; k < nbttaglist4.tagCount(); ++k) {
            NBTCompound nbttagcompound3 = nbttaglist4.getCompoundTagAt(k);
            NBTList nbttaglist5 = nbttagcompound3.getTagList("pos", 6);
            Pos3DTemplate<double> vec3d = new Pos3DTemplate<double>(nbttaglist5.getDoubleAt(0), nbttaglist5.getDoubleAt(1), nbttaglist5.getDoubleAt(2));
            NBTList nbttaglist6 = nbttagcompound3.getTagList("blockPos", 3);
            Pos3D blockpos1 = Pos3D(nbttaglist6.getIntAt(0), nbttaglist6.getIntAt(1), nbttaglist6.getIntAt(2));
    
            if (nbttagcompound3.hasKey("nbt")) {
                NBTCompound nbttagcompound2 = nbttagcompound3.getCompoundTag("nbt");
                this->entities.add(new Template::EntityInfo(vec3d, blockpos1, nbttagcompound2));
            }
        }
    }

private:
    NBTList writeInts(int... values) {
        NBTList nbttaglist = new NBTList();
    
        for (int i: values) { nbttaglist.appendTag(new NBTTagInt(i)); }
    
        return nbttaglist;
    }
    
    NBTList writeDoubles(double... values) {
        NBTList nbttaglist = new NBTList();
    
        for (double d0: values) { nbttaglist.appendTag(new NBTDouble(d0)); }
    
        return nbttaglist;
    }

    class BasicPalette : public Iterable<lce::BlockState> {
    public:
        static constexpr lce::BlockState DEFAULT_BLOCK_STATE = lce::BlocksInit::AIR.getDefaultState();
        const ObjectIntIdentityMap<lce::BlockState> ids;
    private:
        int lastId;
        BasicPalette() { this->ids = new ObjectIntIdentityMap<lce::BlockState>(16); }
    
    public:
        int idFor(lce::BlockState state) {
            int i = this->ids.get(state);
    
            if (i == -1) {
                i = this->lastId++;
                this->ids.put(state, i);
            }
    
            return i;
        }
    
        @Nullable public lce::BlockState stateFor(int id) {
            lce::BlockState iblockstate = this->ids.getByValue(id);
            return iblockstate == null ? DEFAULT_BLOCK_STATE : iblockstate;
        }
    
        Iterator<lce::BlockState> iterator() { return this->ids.iterator(); }
        
        void addMapping(lce::BlockState state, int id) { this->ids.put(state, id); }
    };
    
    

};
